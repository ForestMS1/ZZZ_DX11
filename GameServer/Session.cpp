#include "pch.h"
#include "Session.h"

Session::Session(io_context& io_context)
	: _socket(io_context)
{
}

Session::~Session()
{
}

void Session::start()
{
	do_read();
}

void Session::do_read()
{
	auto self(shared_from_this());

	_socket.async_read_some(boost::asio::buffer(_buffer), [this, self](boost::system::error_code ec, size_t length) {
		if (!ec)
		{
			string received_data(_buffer.data(), length);
			cout << std::format("[{}] 받은 메시지 : {}\n", get_client_info(), received_data);


			// Echo: 받은 데이터를 그대로 전송
			do_write(length);
		}
		else if (ec == boost::asio::error::eof)
		{
			std::cout << std::format("[{}] 클라이언트 정상 연결 종료\n",
				get_client_info());
		}
		else
		{
			std::cout << std::format("[{}] 읽기 오류: {}\n",
				get_client_info(), ec.message());
		}
	});
}

void Session::do_write(size_t length)
{
	auto self(shared_from_this());

	boost::asio::async_write(_socket, boost::asio::buffer(_buffer, length),
		[this, self](boost::system::error_code ec, size_t bytes_written) {
			if (!ec)
			{
				std::cout << std::format("[{}] Echo 응답 전송: {} bytes\n",
					get_client_info(), bytes_written);

				// 다음 메시지를 위해 다시 읽기 시작
				do_read();
			}
			else
			{
				std::cout << std::format("[{}] 쓰기 오류: {}\n",
					get_client_info(), ec.message());
			}
		});
}

string Session::get_client_info()
{
	try
	{
		if (_socket.is_open())
		{
			auto endpoint = _socket.remote_endpoint();
			return std::format("{} : {}", endpoint.address().to_string(), endpoint.port());
		}
	}
	catch (...) {}
	return "알 수 없음";
}
