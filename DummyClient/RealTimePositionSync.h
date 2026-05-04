#pragma once
class RealTimePositionSync
{
public:
	struct PositionData
	{
		uint32_t playerId;
		float x, y, z;
		float vx, vy, vz;
		float rotation;
		uint32_t timestamp;
		uint32_t sequence;
	};
public:
	RealTimePositionSync(boost::asio::io_context& io_context);
	~RealTimePositionSync();

public:
	void UpdatePosition(uint32_t playerId, float x, float y, float z, float vx, float vy, float vz, float rotation);
	void BroadcastPositions();
private:
	void StartSyncTimer();
	void BroadcastPosition(const PositionData& pos);
	uint32_t GetCurrentTimestamp();


private:
	udp::socket _socket;
	boost::asio::steady_timer _timer;

	std::unordered_map<uint32_t, PositionData> _playerPositions;
	std::mutex _positionMutex;

	std::vector<udp::endpoint> _activeClients;
	std::atomic<uint32_t> _sequence;


};

