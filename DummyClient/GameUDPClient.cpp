#include "pch.h"
#include "GameUDPClient.h"

GameUDPClient::GameUDPClient(boost::asio::io_context& io_context, const string& host, const string& port)
	: _socket(io_context)
	, _sequenceNumber(0)
{
	udp::resolver resolver(io_context);
	_serverEndpoint = *resolver.resolve(udp::v4(), host, port).begin();

	_socket.open(udp::v4());

	StartReceive();
	StartHeartbeat();
}

GameUDPClient::~GameUDPClient()
{
}

void GameUDPClient::SendPlayerPosition(float x, float y, float z, float rotX, float rotY, float rotZ)
{
	PlayerPosition pos;
	pos.playerId = 12345;  // 실제로는 클라이언트 ID
	pos.x = x;
	pos.y = y;
	pos.z = z;
	pos.rotX = rotX;
	pos.rotY = rotY;
	pos.rotZ = rotZ;

	pos.timestamp = static_cast<uint32_t>(
		std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count());

	SendGameData(GameDataType::PLAYER_POSITION, &pos, sizeof(pos));
}

void GameUDPClient::SendBroadcastMessage(const string& message)
{
	SendGameData(GameDataType::BROADCAST_MESSAGE, message.data(), message.size());
}

void GameUDPClient::StartReceive()
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

void GameUDPClient::HandleServerMessage(std::size_t bytesTransferred)
{
	if (bytesTransferred < sizeof(GamePacket))
		return;

	auto packet = reinterpret_cast<const GamePacket*>(_recvBuffer.data());
	const char* data = reinterpret_cast<const char*>(packet + 1);

	switch (packet->type)
	{
	case GameDataType::PLAYER_POSITION:
		HandlePlayerPositionUpdate(data, packet->dataSize);
		break;
	case GameDataType::HEARTBEAT:
		HandleHeartbeatResponse();
		break;
	case GameDataType::BROADCAST_MESSAGE:
		HandleBroadcastMessage(data, packet->dataSize);
		break;
	default:
		break;
	}
}

void GameUDPClient::HandlePlayerPositionUpdate(const char* data, uint32_t dataSize)
{
	if (dataSize < sizeof(PlayerPosition))
		return;

	auto pos = reinterpret_cast<const PlayerPosition*>(data);
	std::cout << "Other player " << pos->playerId
		<< " at (" << pos->x << ", " << pos->y << ", " << pos->z << ")"
		<< std::endl;
}

void GameUDPClient::HandleHeartbeatResponse()
{
	_lastHeartbeatResponse = std::chrono::steady_clock::now();
	std::cout << "Heartbeat response received" << std::endl;
}

void GameUDPClient::HandleBroadcastMessage(const char* data, uint32_t dataSize)
{
	std::string message(data, dataSize);
	std::cout << "Broadcast: " << message << std::endl;
}

void GameUDPClient::SendGameData(GameDataType type, const void* data, size_t dataSize)
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

void GameUDPClient::StartHeartbeat()
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

void GameUDPClient::SendHeartbeat()
{
	SendGameData(GameDataType::HEARTBEAT, nullptr, 0);
}





