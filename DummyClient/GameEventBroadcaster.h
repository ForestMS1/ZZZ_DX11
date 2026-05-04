#pragma once

enum class EventType
{
	PLAYER_SPAWN,
	PLAYER_DEATH,
	ITEM_PICKUP,
	EXPLOSION,
	ACHIEVEMENT,
	MATCH_RESULT
};

struct GameEvent
{
	EventType type;
	uint32_t srcPlayerId;
	uint32_t dstPlayerId;
	float x, y, z; // 이벤트 발생 위치
	uint32_t eventData; // 추가 이벤트 데이터
	uint32_t timestamp;
};

class GameEventBroadcaster
{
public:
	GameEventBroadcaster(boost::asio::io_context& io_context, short port);
	~GameEventBroadcaster();
public:
	void TriggerEvent(EventType type, uint32_t srcId, uint32_t dstId, float x, float y, float z, uint32_t data = 0);
	void AddClient(const udp::endpoint& client);
	void RemoveClient(const udp::endpoint& client);

private:
	void BroadcastEvent(const GameEvent& event);
	void LogEvent(const GameEvent& event);
	string GetEventTypeName(EventType type);
	uint32_t GetCurrentTimestamp();

private:
	udp::socket _socket;
	std::set<udp::endpoint> _clients;
	std::mutex _clientsMutex;
};

