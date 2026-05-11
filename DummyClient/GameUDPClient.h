#pragma once
class GameUDPClient
{
public:
	GameUDPClient(boost::asio::io_context& io_context, const string& host, const string& port);
	~GameUDPClient();

public:
	void SendPlayerPosition(float x, float y, float z, float rotX, float rotY, float rotZ);
	void SendBroadcastMessage(const string& message);


private:
	void StartReceive();
	void HandleServerMessage(std::size_t bytesTransferred);
	void HandlePlayerPositionUpdate(const char* data, uint32_t dataSize);
	void HandleHeartbeatResponse();
	void HandleBroadcastMessage(const char* data, uint32_t dataSize);
	void SendGameData(GameDataType type, const void* data, size_t dataSize);
	void StartHeartbeat();
	void SendHeartbeat();


private:
	udp::socket _socket;
	udp::endpoint _serverEndpoint;
	udp::endpoint _senderEndpoint;
	array<char, 1024> _recvBuffer;

	std::atomic<uint32_t> _sequenceNumber;
	std::chrono::steady_clock::time_point _lastHeartbeatResponse;
	std::unique_ptr<boost::asio::steady_timer> _heartbeatTimer;
};

