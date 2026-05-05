#include "pch.h"
#include "GameUDPServer.h"

GameUDPServer::GameUDPServer(io_context& io_context, short port)
	: _socket(io_context, udp::endpoint(boost::asio::ip::make_address("127.0.0.1"), port))
	, _sequenceNumber(0)
{
	StartReceive();
	StartHeartbeat();
}

GameUDPServer::~GameUDPServer()
{
}

void GameUDPServer::BroadcastToAllClients(const string& message)
{
	lock_guard<mutex> lock(_clientMutex);
	for (auto& client : _activeClients)
	{
		SendToClient(client.second, message);
	}
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

	auto packet = reinterpret_cast<const GamePacket*>(_recvBuffer.data());

	UpdateClientInfo(_remoteEndpoint, packet->sequenceNumber);

	ProcessGamePacket(packet, bytesTransferred);
}

void GameUDPServer::UpdateClientInfo(const udp::endpoint& endpoint, uint32_t sequence)
{
	lock_guard<mutex> lock(_clientMutex);

	string clientKey = endpoint.address().to_string() + ":" + to_string(endpoint.port());

	auto iter = _activeClients.find(clientKey);
	if (iter == _activeClients.end())
	{
		std::cout << "New Client Connected: " << clientKey << std::endl;
	}

	auto& client = _activeClients[clientKey];
	client.endpoint = endpoint;
	client.lastSeen = std::chrono::steady_clock::now();
	client.lastSequence = sequence;
}

void GameUDPServer::ProcessGamePacket(const GamePacket* packet, std::size_t totalSize)
{
	const char* data = reinterpret_cast<const char*>(packet + 1);

	switch (packet->type)
	{
	case GameDataType::OBJECT_SYNC:
		std::cout << "OBJECT_SYNC 메시지 받음" << std::endl;
		HandlePlayerPosition(packet, data, packet->dataSize);
		break;
	case GameDataType::HEARTBEAT:
		std::cout << "HEARTBEAT 메시지 받음" << std::endl;
		HandleHeartBeat();
		break;
	case GameDataType::BROADCAST_MESSAGE:
		HandleBroadcastMessage(data, packet->dataSize);
		break;
	default:
		std::cout << "Unknown packet type: "
			<< static_cast<int>(packet->type) << std::endl;
		break;
	}
}

void GameUDPServer::HandlePlayerPosition(const GamePacket* packet, const char* data, uint32_t dataSize)
{
	if (dataSize < sizeof(PlayerPosition))
		return;

	auto pos = reinterpret_cast<const PlayerPosition*>(data);

	// 1. 누가 보냈는지 식별 (IP와 Port 추출)
	std::string clientAddr = _remoteEndpoint.address().to_string();
	unsigned short clientPort = _remoteEndpoint.port();

	// 2. 로그 출력 (형식: [IP:Port] Sync - Pos: (x, y, z))
	// 숫자가 너무 길어지지 않게 소수점 2자리 정도로 제한하면 더 깔끔합니다.
	printf("[%s:%d] OBJECT_SYNC -> Pos: (%.2f, %.2f, %.2f) | Rot: (%.2f, %.2f, %.2f)\n",
		clientAddr.c_str(),
		clientPort,
		pos->position.x, pos->position.y, pos->position.z,
		pos->rotation.x, pos->rotation.y, pos->rotation.z);

	// 다른 클라이언트들에게 브로드캐스트
	BroadcastPlayerPosition(packet, *pos);
}

void GameUDPServer::HandleHeartBeat()
{
	SendHeartbeatResponse(_remoteEndpoint);
}

void GameUDPServer::HandleBroadcastMessage(const char* data, uint32_t dataSize)
{
	string message(data, dataSize);
	std::cout << "Broadcast Message : " << message << std::endl;
	BroadcastToAllClients(message);
}

void GameUDPServer::BroadcastPlayerPosition(const GamePacket* packet, const PlayerPosition& pos)
{
	GamePacket newPacket;
	newPacket.type = GameDataType::OBJECT_SYNC;
	newPacket.objectId = packet->objectId;
	newPacket.sequenceNumber = ++_sequenceNumber;
	newPacket.syncFlags = packet->syncFlags;
	newPacket.dataSize = sizeof(PlayerPosition);

	auto buffer = make_shared<vector<char>>(sizeof(GamePacket) + sizeof(PlayerPosition));
	::memcpy(buffer->data(), &newPacket, sizeof(GamePacket));
	::memcpy(buffer->data() + sizeof(GamePacket), &pos, sizeof(PlayerPosition));	

	lock_guard<mutex> lock(_clientMutex);
	for (const auto& client : _activeClients)
	{
		_socket.async_send_to(boost::asio::buffer(*buffer), client.second.endpoint,
			[buffer](const boost::system::error_code&, size_t){});
	}
}

void GameUDPServer::SendToClient(const clientInfo& client, const string& message)
{
	GamePacket packet;
	packet.type = GameDataType::BROADCAST_MESSAGE;
	packet.sequenceNumber = ++_sequenceNumber;
	packet.dataSize = static_cast<uint32_t>(message.size());

	vector<char> buffer(sizeof(GamePacket) + message.size());
	::memcpy(buffer.data(), &packet, sizeof(GamePacket));
	::memcpy(buffer.data() + sizeof(GamePacket), message.data(), message.size());

	auto sendBuffer = make_shared<vector<char>>(std::move(buffer));
	_socket.async_send_to(boost::asio::buffer(*sendBuffer), client.endpoint,
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
	lock_guard<mutex> lock(_clientMutex);
	auto now = std::chrono::steady_clock::now();
	auto timeout = std::chrono::seconds(30);

	for (auto iter = _activeClients.begin(); iter != _activeClients.end();)
	{
		if (now - iter->second.lastSeen > timeout)
		{
			std::cout << "Client timeout: " << iter->first << std::endl;
			iter = _activeClients.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}