#pragma once

class ServerPlayer;

class Session : public enable_shared_from_this<Session>
{
public:
	Session(udp::endpoint endpoint, uint32_t sessionId);
	~Session();

public:
	// Getter / Setter
	udp::endpoint GetEndpoint() const { return _endpoint; }
	uint32_t GetSessionId() const { return _sessionId; }

	uint32_t GetPlayerID() const;

	shared_ptr<ServerPlayer> GetPlayer() { return _player; }
	void SetPlayer(shared_ptr<ServerPlayer> player);

	// 하트비트 및 시퀀스 관리
	void UpdateLastSeen() { _lastSeen = std::chrono::steady_clock::now(); }
	auto GetLastSeen() const { return _lastSeen; }

	bool IsSequenceValid(uint32_t newSequence);

	// GameUDPServer에서 기능을 제공하거나, Session이 소켓 참조를 받아 처리
	void Send(udp::socket& socket, const string& packetData);

private:

private:
	shared_ptr<ServerPlayer> _player = nullptr;

	udp::endpoint _endpoint;
	std::chrono::steady_clock::time_point _lastSeen;
	uint32_t _lastSequence = 0;
	uint32_t _sessionId = 0;

	std::chrono::seconds _timeoutDuration { 300 }; // 5분
};

