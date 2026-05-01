#include "pch.h"
#include "SimpleChatServer.h"

SimpleChatServer::SimpleChatServer(unsigned short port)
	:_acceptor(_io_context, tcp::endpoint(tcp::v4(), port))
{
	std::cout << std::format("채팅 서버가 포트 {}에서 시작되었습니다.\n", port);
}

SimpleChatServer::~SimpleChatServer()
{
	stop();
}

void SimpleChatServer::run()
{
	std::cout << "채팅 서버 실행 중... (Ctrl+C로 종료)\n" << std::endl;

	try
	{
		while (_serverRunning)
		{
			auto clientSocket = make_shared<tcp::socket>(_io_context);

			boost::system::error_code accept_error;
			_acceptor.accept(*clientSocket, accept_error);

			if (accept_error)
			{
				if (_serverRunning)
				{
					std::cerr << "Accept 오류: " << accept_error.message() << std::endl;
				}
				break;
			}

			std::cout << "새 클라이언트 연결: " << clientSocket->remote_endpoint() << std::endl;

			// 클라이언트 목록에 추가
			{
				std::lock_guard<std::mutex> lock(_clientMutex);
				_activeClients.push_back(clientSocket);
			}

			// 새 스레드에서 클라이언트 처리
			auto clientThread = make_shared<std::thread>(&SimpleChatServer::handle_client, this, clientSocket);

			_clientThreads.push_back(clientThread);

			// 종료된 스레드 정리
			cleanup_finished_threads();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "서버 오류: " << e.what() << std::endl;
	}
}

void SimpleChatServer::stop()
{
	_serverRunning = false;

	//모든 클라이언트 연결 종료
	{
		std::lock_guard<std::mutex> lock(_clientMutex);
		for (auto& client : _activeClients)
		{
			if (client->is_open())
			{
				boost::system::error_code ec;
				client->close(ec);
			}
		}
		_activeClients.clear();
	}

	// acceptor 종료
	boost::system::error_code ec;
	_acceptor.close(ec);

	//모든 클라이언트 스레드 종료 대기
	for (auto& thread : _clientThreads)
	{
		if (thread->joinable())
		{
			thread->join();
		}
	}
	_clientThreads.clear();

	std::cout << "채팅 서버가 종료되었습니다." << std::endl;
}

void SimpleChatServer::handle_client(std::shared_ptr<tcp::socket> client_socket)
{
	try
	{
		std::string client_address = client_socket->remote_endpoint().address().to_string();
		std::cout << std::format("[{}] 클라이언트 핸들러 시작\n", client_address);

		// 환영 메시지 전송
		std::string welcome_msg = "채팅 서버에 오신 것을 환영합니다!\n";
		boost::system::error_code send_error;
		write(*client_socket, boost::asio::buffer(welcome_msg), send_error);

		if (send_error) {
			std::cout << std::format("[{}] 환영 메시지 전송 실패\n", client_address);
			return;
		}

		while (_serverRunning && client_socket->is_open())
		{
			std::array<char, 1024> buffer;
			buffer.fill(0);

			boost::system::error_code receive_error;
			size_t length = client_socket->read_some(
				boost::asio::buffer(buffer), receive_error
			);

			if (receive_error == boost::asio::error::eof) {
				std::cout << std::format("[{}] 클라이언트 정상 종료\n", client_address);
				break;
			}
			else if (receive_error) {
				std::cout << std::format("[{}] 수신 오류: {}\n",
					client_address, receive_error.message());
				break;
			}

			if (length > 0)
			{
				string message(buffer.data(), length);

				//줄바꿈 제거
				message.erase(std::remove(message.begin(), message.end(), '\n'), message.end());
				message.erase(std::remove(message.begin(), message.end(), '\r'), message.end());

				if (!message.empty())
				{
					std::cout << std::format("[{}] 메시지: {}\n", client_address, message);

					// 모든 클라이언트에게 브로드캐스트
					broadcast_message(client_address, message);
				}
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "클라이언트 처리 중 예외: " << e.what() << std::endl;
	}

	// 클라이언트 목록에서 제거
	remove_client(client_socket);

	std::string client_address = "알 수 없음";
	try
	{
		if (client_socket->is_open()) {
			client_address = client_socket->remote_endpoint().address().to_string();
		}
	}
	catch (...) {}

	std::cout << std::format("[{}] 클라이언트 핸들러 종료\n", client_address);
}

void SimpleChatServer::broadcast_message(const std::string& sender_address, const std::string& message)
{
	std::string broadcast_msg = std::format("[{}]: {}\n", sender_address, message);

	std::lock_guard<std::mutex> lock(_clientMutex);

	auto iter = _activeClients.begin();
	while (iter != _activeClients.end())
	{
		auto client = *iter;

		if (!client->is_open())
		{
			// 닫힌 소켓 제거
			iter = _activeClients.erase(iter);
			continue;
		}

		boost::system::error_code send_error;
		write(*client, boost::asio::buffer(broadcast_msg), send_error);

		if (send_error)
		{
			// 전송 실패한 클라이언트 제거
			boost::system::error_code close_error;
			client->close(close_error);
			iter = _activeClients.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	std::cout << std::format("브로드캐스트 완료: {} ({}명에게 전송)\n",
		message, _activeClients.size());
}

void SimpleChatServer::remove_client(std::shared_ptr<tcp::socket> client_socket)
{
	std::lock_guard<mutex> lock(_clientMutex);

	_activeClients.erase(
		std::remove(_activeClients.begin(), _activeClients.end(), client_socket),
		_activeClients.end()
	);
}

void SimpleChatServer::cleanup_finished_threads()
{
	auto iter = _clientThreads.begin();
	while (iter != _clientThreads.end())
	{
		// 스레드가 종료되었는지 확인 (실제로는 더 정교한 방법이 필요)
		++iter;
	}
}
