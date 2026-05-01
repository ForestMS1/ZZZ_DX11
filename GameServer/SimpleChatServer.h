#pragma once

class SimpleChatServer
{
public:
	SimpleChatServer(unsigned short port);
	~SimpleChatServer();

public:

	void run();
	void stop();



private:
	void handle_client(std::shared_ptr<tcp::socket> client_socket);
	void broadcast_message(const std::string& sender_address, const std::string& message);
	void remove_client(std::shared_ptr<tcp::socket> client_socket);
	void cleanup_finished_threads();

private:
	io_context _io_context;
	tcp::acceptor _acceptor;

	vector<shared_ptr<std::thread>> _clientThreads;
	vector<shared_ptr<tcp::socket>> _activeClients; // 공유자원

	mutex _clientMutex;
	std::atomic<bool> _serverRunning { true };
};

// Acceptor → 클라이언트 연결 대기
// Socket → 클라이언트 통신 담당
// Thread per client → 클라이언트별 독립 처리
// Broadcast → 받은 메시지를 모든 클라이언트에 전송
// Mutex 보호 → 다중 클라이언트 동시성 제어
// Graceful shutdown → 서버와 클라이언트 정리 후 종료

