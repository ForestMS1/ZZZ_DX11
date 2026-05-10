#include "pch.h"
#include "GameUDPServer.h"
#include "Session.h"
#include "SessionManager.h"
#include "ServerPlayer.h"
GameUDPServer::GameUDPServer(io_context& io_context, short port)
	: _socket(io_context, udp::endpoint(udp::v4(), port))
	, _sequenceNumber(0)
{
	StartReceive();
	StartHeartbeat();
}

GameUDPServer::~GameUDPServer()
{
}

void GameUDPServer::Update()
{
	auto previousTime = std::chrono::steady_clock::now();
	const std::chrono::milliseconds tickRate(33); // 약 30 FPS

	while (_isRunning)
	{
		auto currentTime = std::chrono::steady_clock::now();
		std::chrono::duration<float> elapsedTime = currentTime - previousTime;
		float deltaTime = elapsedTime.count();
		previousTime = currentTime;

		this_thread::sleep_until(currentTime + tickRate);
	}
}

void GameUDPServer::BroadcastToAllClients(const string& message)
{
	GamePacket packet;
	packet.type = GameDataType::BROADCAST_MESSAGE;
	packet.sequenceNumber = ++_sequenceNumber;
	packet.dataSize = static_cast<uint32_t>(message.size());

	auto buffer = make_shared<vector<char>>(sizeof(GamePacket) + message.size());
	::memcpy(buffer->data(), &packet, sizeof(GamePacket));
	::memcpy(buffer->data() + sizeof(GamePacket), message.data(), message.size());

	SessionManager::Get().ForEachSession([this, buffer](shared_ptr<Session> target) {
		_socket.async_send_to(boost::asio::buffer(*buffer), target->GetEndpoint(),
			[buffer](const boost::system::error_code&, std::size_t) {});
		});
}

void GameUDPServer::StartReceive()
{
	_socket.async_receive_from(
		boost::asio::buffer(_recvBuffer), _remoteEndpoint,
		[this](const boost::system::error_code& error, std::size_t bytes)
		{
			if (!error)
			{
				HandleClientMessage(bytes);
			}
			StartReceive();
		}
	);
}

void GameUDPServer::HandleClientMessage(size_t bytesTransferred)
{
	if (bytesTransferred < sizeof(GamePacket))
		return;

	// 세션 식별 및 갱신 (기존 UpdateClientInfo 대체)
	auto session = SessionManager::Get().GetOrCreateSession(_remoteEndpoint);
	session->UpdateLastSeen();

	auto packet = reinterpret_cast<const GamePacket*>(_recvBuffer.data());

	// 시퀀스 번호 검증 (오래된 패킷 드랍)
	if (!session->IsSequenceValid(packet->sequenceNumber))
		return;

	ProcessGamePacket(session, packet, bytesTransferred);
}

void GameUDPServer::HandleEnterScene(shared_ptr<Session> newSession) {
	// 서버 메모리에 새로운 플레이어 객체 생성
	uint32_t newId = SessionManager::Get().GenerateObjectId(); // 고유 ID 생성기
	auto newPlayer = make_shared<ServerPlayer>(newId, "유저이름", 0.f, 0.f, 0.f);
	//newPlayer->SetId(newId);
	newPlayer->SetFactoryName("TestCorin"); // 유저가 선택한 클래스 이름
	//newPlayer->SetPosition(0.f, 0.f, 0.f); // 초기 스폰 위치

	// 세션과 플레이어 연결
	newSession->SetPlayer(newPlayer);

	// [본인에게 전송] "너의 캐릭터는 이거야!"
	SendEnterScene(newSession, newPlayer, true);

	// 기존 유저들과의 상호 동기화
	SessionManager::Get().ForEachSession([&](shared_ptr<Session> otherSession) {
		if (otherSession == newSession) return;

		auto otherPlayer = otherSession->GetPlayer();
		if (!otherPlayer) return;

		// A. [기존 유저들에게] "새로운 유저가 나타났다!" 알림
		SendEnterScene(otherSession, newPlayer, false);

		// B. [새 유저에게] "여기에 이미 이런 사람들이 있어" 정보 전달
		SendEnterScene(newSession, otherPlayer, false);
		});
}

void GameUDPServer::SendEnterScene(shared_ptr<Session> targetSession, shared_ptr<ServerPlayer> player, bool isMine) {
	S_EnterScene packet;
	::ZeroMemory(&packet, sizeof(S_EnterScene));

	packet.isMine = isMine;
	packet.player.objectId = player->GetId();

	// 로그로 확인
	string factoryName = player->GetFactoryName();
	strcpy_s(packet.player.className, factoryName.c_str());

	printf("[SendEnterScene] ID: %d, Class: %s, Target: %s\n",
		packet.player.objectId, packet.player.className,
		targetSession->GetEndpoint().address().to_string().c_str());

	Vec3 pos = player->GetPosition();
	packet.player.posX = pos.x;
	packet.player.posY = pos.y;
	packet.player.posZ = pos.z;

	// GamePacket 헤더 조립 후 전송
	GamePacket header;
	header.type = GameDataType::ENTER_SCENE;
	header.dataSize = sizeof(S_EnterScene);
	header.sequenceNumber = ++_sequenceNumber;

	vector<char> buffer(sizeof(GamePacket) + sizeof(S_EnterScene));
	memcpy(buffer.data(), &header, sizeof(GamePacket));
	memcpy(buffer.data() + sizeof(GamePacket), &packet, sizeof(S_EnterScene));

	auto sendBuffer = make_shared<vector<char>>(std::move(buffer));
	_socket.async_send_to(boost::asio::buffer(*sendBuffer), targetSession->GetEndpoint(),
		[sendBuffer](const boost::system::error_code&, size_t) {});
}

void GameUDPServer::ProcessGamePacket(shared_ptr<Session> session, const GamePacket* packet, std::size_t totalSize)
{
	const char* data = reinterpret_cast<const char*>(packet + 1);

	switch (packet->type)
	{
	case GameDataType::OBJECT_SYNC:
		std::cout << "OBJECT_SYNC 메시지 받음" << std::endl;
		HandleObjectSync(session, packet, data, packet->dataSize);
		break;
	case GameDataType::HEARTBEAT:
		std::cout << "HEARTBEAT 메시지 받음" << std::endl;
		HandleHeartBeat(session);
		break;
	case GameDataType::BROADCAST_MESSAGE:
		HandleBroadcastMessage(session, data, packet->dataSize);
		break;
	case GameDataType::ENTER_SCENE_REQUEST:
		HandleEnterScene(session);
		break;
	default:
		std::cout << "Unknown packet type: "
			<< static_cast<int>(packet->type) << std::endl;
		break;
	}
}

void GameUDPServer::HandleObjectSync(shared_ptr<Session> session, const GamePacket* packet, const char* payload, uint32_t dataSize)
{
	auto player = session->GetPlayer();


	// 누가 보냈는지 식별 (IP와 Port 추출)
	std::string clientAddr = _remoteEndpoint.address().to_string();
	unsigned short clientPort = _remoteEndpoint.port();

	uint32_t offset = 0;

	// 2. 위치 데이터 로그 (첫 번째 데이터)
	if (packet->syncFlags & 0x01) // SyncFlag::POSITION
	{
		const PlayerPosition* pos = reinterpret_cast<const PlayerPosition*>(payload + offset);
		if (player)
			player->SetPosition(pos->posX, pos->posY, pos->posZ);

		printf("   [POS] (%.2f, %.2f, %.2f)\n", pos->posX, pos->posY, pos->posZ);
		offset += sizeof(PlayerPosition); // 40바이트 이동
	}

	// 3. 애니메이션 데이터 로그 (두 번째 데이터)
	if (packet->syncFlags & 0x02) // SyncFlag::ANIMATION
	{
		// offset이 정확해야 이 캐스팅이 성공
		const AnimationData* anim = reinterpret_cast<const AnimationData*>(payload + offset);

		printf("   [ANIM] Index: %d | Params: %d\n",
			anim->animIndex, anim->paramCount);

		// 파라미터가 있다면 첫 번째 파라미터 해시값만 확인
		if (anim->paramCount > 0)
		{
			const AnimParamData* param = reinterpret_cast<const AnimParamData*>(anim + 1);
			printf("      ㄴ First Param Hash: 0x%X | Value: %.2f\n", param->paramHash, param->value);
		}
	}

	// 다른 클라이언트들에게 브로드캐스트
	BroadcastObjectSync(session, packet, payload, dataSize);
}
void GameUDPServer::HandleHeartBeat(shared_ptr<Session> session)
{
	SendHeartbeatResponse(session->GetEndpoint());
}

void GameUDPServer::HandleBroadcastMessage(shared_ptr<Session> session, const char* data, uint32_t dataSize)
{
	string message(data, dataSize);
	std::cout << "Broadcast Message : " << message << std::endl;
	BroadcastToAllClients(message);
}

void GameUDPServer::BroadcastObjectSync(shared_ptr<Session> session, const GamePacket* packet, const char* payload, uint32_t dataSize)
{
	GamePacket newPacket;
	newPacket.type = GameDataType::OBJECT_SYNC;
	newPacket.objectId = packet->objectId;
	newPacket.sequenceNumber = packet->sequenceNumber;
	newPacket.syncFlags = packet->syncFlags;
	newPacket.dataSize = dataSize;

	auto buffer = make_shared<vector<char>>(sizeof(GamePacket) + dataSize);
	::memcpy(buffer->data(), &newPacket, sizeof(GamePacket));
	if (dataSize > 0)
	{
		::memcpy(buffer->data() + sizeof(GamePacket), payload, dataSize);
	}

	// SessionManager를 통해 모든 세션에게 전송
	SessionManager::Get().ForEachSession([this, buffer, session](shared_ptr<Session> target) {
		// (옵션) 나 자신에게는 다시 보내지 않도록 체크
		if (target == session) return;

		_socket.async_send_to(boost::asio::buffer(*buffer), target->GetEndpoint(),
			[buffer](const boost::system::error_code&, size_t) {});
		});
}

void GameUDPServer::SendToClient(shared_ptr<Session> session, const string& message)
{
	GamePacket packet;
	packet.type = GameDataType::BROADCAST_MESSAGE;
	packet.sequenceNumber = ++_sequenceNumber;
	packet.dataSize = static_cast<uint32_t>(message.size());

	vector<char> buffer(sizeof(GamePacket) + message.size());
	::memcpy(buffer.data(), &packet, sizeof(GamePacket));
	::memcpy(buffer.data() + sizeof(GamePacket), message.data(), message.size());

	auto sendBuffer = make_shared<vector<char>>(std::move(buffer));
	_socket.async_send_to(boost::asio::buffer(*sendBuffer), session->GetEndpoint(),
		[sendBuffer](const boost::system::error_code&, std::size_t) {});
}

void GameUDPServer::SendHeartbeatResponse(const udp::endpoint& endpoint)
{
	auto packet = make_shared<GamePacket>();
	packet->type = GameDataType::HEARTBEAT;
	packet->sequenceNumber = ++_sequenceNumber;
	packet->dataSize = 0;

	_socket.async_send_to(boost::asio::buffer(packet.get(), sizeof(GamePacket)), endpoint,
		[packet](const boost::system::error_code&, std::size_t) {});
}

void GameUDPServer::StartHeartbeat()
{
	_heartBeatTimer = make_unique <boost::asio::steady_timer>(
		_socket.get_executor(), std::chrono::seconds(10));

	_heartBeatTimer->async_wait([this](const boost::system::error_code& ec)
		{
			if (!ec)
			{
				CleanupInactiveClients();
				StartHeartbeat();
			}
		}
	);
}

void GameUDPServer::CleanupInactiveClients()
{
	// SessionManager 내부에서 정의한 타임아웃 정리 함수 호출
	SessionManager::Get().CleanupInactiveSessions(std::chrono::seconds(30));
}