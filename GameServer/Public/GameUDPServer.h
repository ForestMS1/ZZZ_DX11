#pragma once

class Session;
class ServerPlayer;

class GameUDPServer
{
public:
	GameUDPServer(io_context& io_context, short port);
	~GameUDPServer();

public:
	void Update();
	void BroadcastToAllClients(const string& message);



private:

	void StartReceive();
	void HandleClientMessage(size_t bytesTransferred);
	void HandleEnterScene(shared_ptr<Session> newSession);
	void SendEnterScene(shared_ptr<Session> targetSession, shared_ptr<ServerPlayer> player, bool isMine);
	void ProcessGamePacket(shared_ptr<Session> session, const GamePacket* packet, std::size_t totalSize);
	void HandleObjectSync(shared_ptr<Session> session, const GamePacket* packet, const char* paylod, uint32_t dataSize);
	void HandleHeartBeat(shared_ptr<Session> session);
	void HandleBroadcastMessage(shared_ptr<Session> session, const char* data, uint32_t dataSize);
	void BroadcastObjectSync(shared_ptr<Session> session, const GamePacket* packet, const char* payload, uint32_t dataSize);
	void SendToClient(shared_ptr<Session> session, const string& message);
	void SendHeartbeatResponse(const udp::endpoint& endpoint);
	void StartHeartbeat();
	void CleanupInactiveClients();
	


private:
	udp::socket _socket;
	udp::endpoint _remoteEndpoint;
	array<char, 1024> _recvBuffer;

	atomic<uint32_t> _sequenceNumber;
	unique_ptr<boost::asio::steady_timer> _heartBeatTimer;


private:
	bool _isRunning = true;
};

