#include "pch.h"
#include "NetworkManager.h"
#include "GameObject.h"
#include "NetworkTransformView.h"
#include "NetworkView.h"
NetworkManager::NetworkManager(const string& host, const string& port)
	: _socket(_io_context)
	, _sequenceNumber(0)
{
	boost::asio::ip::udp::resolver resolver(_io_context);
	_serverEndpoint = *resolver.resolve(boost::asio::ip::udp::v4(), host, port).begin();

	_socket.open(boost::asio::ip::udp::v4());

	_workerThread = std::jthread([this](std::stop_token stoken) {
		std::cout << "Network Worker Thread Started." << std::endl;

		// 작업이 없어도 run()이 종료되지 않도록 유지
		auto workGuard = boost::asio::make_work_guard(_io_context);

		// io_context 실행 (비동기 작업들이 여기서 처리됨)
		_io_context.run();

		std::cout << "Network Worker Thread Stopped." << std::endl;
		});

	StartReceive();
	StartHeartbeat();
}

NetworkManager::~NetworkManager()
{
	Stop();
}

void NetworkManager::Stop() {
	_io_context.stop(); // run()을 중단시킴
	if (_workerThread.joinable())
		_workerThread.join();
}

void NetworkManager::SendBroadcastMessage(const string& message)
{
	SendGameData(GameDataType::BROADCAST_MESSAGE, message.data(), message.size());
}

void NetworkManager::AddNetworkView(shared_ptr<NetworkView> networkView)
{
	if (networkView == nullptr) 
		return; 

	uint32_t objId = networkView->GetNetworkID();

	_networkViewMap.emplace(objId, networkView);
}

// 클라 -> 서버 패킷 전송
void NetworkManager::SendObjectSync(shared_ptr<GamePacket> header, const vector<uint8_t>& payload)
{
	// 헤더와 바디를 각각 버퍼로 묶어서 한 번에 전송
	// 헤더와 바디를 모두 담을 수 있는 하나의 버퍼 생성
	auto sendBuffer = make_shared<vector<uint8_t>>();

	// 버퍼 크기 예약 (헤더 크기 + 데이터 크기)
	sendBuffer->resize(sizeof(GamePacket) + payload.size());

	// 헤더 복사
	memcpy(sendBuffer->data(), header.get(), sizeof(GamePacket));

	// 페이로드 복사
	if (!payload.empty())
	{
		memcpy(sendBuffer->data() + sizeof(GamePacket), payload.data(), payload.size());
	}

	// 전송 (sendBuffer 전체를 하나의 asio::buffer로 넘김)
	_socket.async_send_to(boost::asio::buffer(*sendBuffer), _serverEndpoint,
		[header, sendBuffer](const boost::system::error_code& ec, std::size_t bytes) {});
}

void NetworkManager::StartReceive()
{
	_socket.async_receive_from(boost::asio::buffer(_recvBuffer), _senderEndpoint,
		[this](const boost::system::error_code& error, std::size_t bytes)
		{
			if (!error)
			{
				HandleServerMessage(bytes);
			}
			StartReceive();
		}
	);
}

// 서버 -> 클라로 온 패킷 분류
void NetworkManager::HandleServerMessage(std::size_t bytesTransferred)
{
	// 최소 헤더 크기 체크
	if (bytesTransferred < sizeof(GamePacket))
		return;

	auto packet = reinterpret_cast<const GamePacket*>(_recvBuffer.data());

	// 패킷에 명시된 데이터 크기가 실제 전송된 크기와 맞는지 검증
	if (bytesTransferred < sizeof(GamePacket) + packet->dataSize)
		return;

	// 데이터 시작 지점 (헤더 바로 뒤)
	const char* payload = _recvBuffer.data() + sizeof(GamePacket);

	switch (packet->type)
	{
	case GameDataType::OBJECT_SYNC:
		HandleObjectSync(*packet, payload);
		break;
	case GameDataType::HEARTBEAT:
		HandleHeartbeatResponse();
		break;
	case GameDataType::BROADCAST_MESSAGE:
		HandleBroadcastMessage(payload, packet->dataSize);
		break;
	default:
		break;
	}
}

void NetworkManager::HandleObjectSync(const GamePacket& header, const char* payload)
{
	// 씬에서 해당 ID를 가진 NetworkView를 찾음
	uint32_t objID = header.objectId;
	auto view = FindNetworkView(header.objectId);
	if (view == nullptr) 
		return;
	
	// 시퀀스 번호 체크 (UDP 패킷 순서 뒤바뀜 방지)
	if (header.sequenceNumber <= view->GetLastSequence())
		return;

	view->SetLastSequence(header.sequenceNumber);

	// 내 캐릭터라면 서버의 데이터를 무시 (내가 보낸 게 돌아온 것)
	//if (view->IsMine()) return;

	// syncFlags를 체크하여 어떤 데이터가 들어있는지 확인
	uint32_t offset = 0;

	if (header.syncFlags & SyncFlag::POSITION) {
		// 찾은 NetworkView를 통해 실제 컴포넌트에 데이터 전달
		// (직접 전달하거나, View 내부의 인터페이스를 호출)
		if (auto transformView = view->GetGameObject()->GetScript<NetworkTransformView>()) {
			transformView->OnDeserialize(payload + offset);
		}
		offset += sizeof(PlayerPosition);
	}

	// 애니메이션 등 추가 데이터가 있다면 동일하게 처리
	// if (header.syncFlags & SyncFlag::ANIMATION) { ... }
}

void NetworkManager::HandleHeartbeatResponse()
{
	_lastHeartbeatResponse = std::chrono::steady_clock::now();
	std::cout << "Heartbeat response received" << std::endl;
}

void NetworkManager::HandleBroadcastMessage(const char* data, uint32_t dataSize)
{
	std::string message(data, dataSize);
	std::cout << "Broadcast: " << message << std::endl;
}

void NetworkManager::SendGameData(GameDataType type, const void* data, size_t dataSize)
{
	GamePacket packet;
	packet.type = type;
	packet.sequenceNumber = ++_sequenceNumber;
	packet.dataSize = static_cast<uint32_t>(dataSize);

	vector<char> buffer(sizeof(GamePacket) + dataSize);
	::memcpy(buffer.data(), &packet, sizeof(GamePacket));
	if (dataSize > 0)
	{
		::memcpy(buffer.data(), data, dataSize);
	}

	auto sendBuffer = make_shared<vector<char>>(std::move(buffer));
	_socket.async_send_to(boost::asio::buffer(*sendBuffer), _serverEndpoint,
		[sendBuffer](const boost::system::error_code& ec, std::size_t bytes)
		{
			if (ec)
			{
				std::cerr << "Send error: " << ec.message() << std::endl;
			}
		}
	);
}

void NetworkManager::StartHeartbeat()
{
	_heartbeatTimer = make_unique<boost::asio::steady_timer>(
		_socket.get_executor(), std::chrono::seconds(5)
	);

	_heartbeatTimer->async_wait([this](const boost::system::error_code& ec)
		{
			if (!ec)
			{
				SendHeartbeat();
				StartHeartbeat();
			}
		});
}

void NetworkManager::SendHeartbeat()
{
	SendGameData(GameDataType::HEARTBEAT, nullptr, 0);
}





