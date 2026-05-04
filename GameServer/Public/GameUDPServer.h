#pragma once

class GameUDPServer
{
public:
	GameUDPServer(io_context& io_context, short port);
	~GameUDPServer();

public:
	void BroadcastToAllClients(const string& message);



private:
	struct clientInfo
	{
		udp::endpoint endpoint;
		std::chrono::steady_clock::time_point lastSeen;
		uint32_t lastSequence;
	};

	void StartReceive();
	void HandleClientMessage(size_t bytesTransferred);
	void UpdateClientInfo(const udp::endpoint& endpoint, uint32_t sequence);
	void ProcessGamePacket(const GamePacket* packet, std::size_t totalSize);
	void HandlePlayerPosition(const char* data, uint32_t dataSize);
	void HandleHeartBeat();
	void HandleBroadcastMessage(const char* data, uint32_t dataSize);
	void BroadcastPlayerPosition(const PlayerPosition& pos);
	void SendToClient(const clientInfo& client, const string& message);
	void SendHeartbeatResponse(const udp::endpoint& endpoint);
	void StartHeartbeat();
	void CleanupInactiveClients();


private:
	udp::socket _socket;
	udp::endpoint _remoteEndpoint;
	array<char, 1024> _recvBuffer;

	unordered_map<string, clientInfo> _activeClients;
	mutex _clientMutex;

	atomic<uint32_t> _sequenceNumber;
	unique_ptr<boost::asio::steady_timer> _heartBeatTimer;
};

