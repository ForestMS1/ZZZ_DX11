#pragma once

class NetworkView;

class NetworkManager
{
private:
	NetworkManager(const std::string& host, const std::string& port);
	~NetworkManager();


	NetworkManager(const NetworkManager&) = delete;
	NetworkManager& operator=(const NetworkManager&) = delete;

public:
	static NetworkManager& Get(
		const std::string& host = "127.0.0.1",
		const std::string& port = "9000") {
		static NetworkManager instance(host, port);
		return instance;
	}
public:
	boost::asio::io_context& GetContext() { return _io_context; }

public:
	void SendObjectSync(shared_ptr<GamePacket> header, const vector<uint8_t>& payload);
	void SendBroadcastMessage(const string& message);

	void AddNetworkView(shared_ptr<NetworkView> networkView);

private:
	void StartReceive();
	void HandleServerMessage(std::size_t bytesTransferred);
	void HandleObjectSync(const GamePacket& header, const char* payload);
	void HandleHeartbeatResponse();
	void HandleBroadcastMessage(const char* data, uint32_t dataSize);
	void SendGameData(GameDataType type, const void* data, size_t dataSize);
	void StartHeartbeat();
	void SendHeartbeat();


private:
	boost::asio::io_context _io_context;

	boost::asio::ip::udp::socket _socket;
	boost::asio::ip::udp::endpoint _serverEndpoint;
	boost::asio::ip::udp::endpoint _senderEndpoint;
	array<char, 1024> _recvBuffer;

	std::atomic<uint32_t> _sequenceNumber;
	std::chrono::steady_clock::time_point _lastHeartbeatResponse;
	std::unique_ptr<boost::asio::steady_timer> _heartbeatTimer;

private:
	std::unordered_map<uint32_t, shared_ptr<NetworkView>>	_networkViewMap;

	shared_ptr<NetworkView> FindNetworkView(uint32_t id) {
		auto it = _networkViewMap.find(id);
		return (it != _networkViewMap.end()) ? it->second : nullptr;
	}

private:
	std::jthread _workerThread;
	void Stop();
};

