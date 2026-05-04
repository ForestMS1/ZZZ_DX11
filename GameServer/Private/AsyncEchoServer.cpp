#include "pch.h"
#include "AsyncEchoServer.h"
#include "Session.h"
AsyncEchoServer::AsyncEchoServer(io_context& io_context, unsigned short port)
	: _io_context(io_context)
	, _acceptor(_io_context, tcp::endpoint(tcp::v4(), port))
{
	std::cout << std::format("비동기 Echo 서버 시작 - 포트: {}\n", port);
	start_accept();
}

AsyncEchoServer::~AsyncEchoServer()
{
}

void AsyncEchoServer::start_accept()
{
	// 새로운 세션 생성
	auto new_session = make_shared<Session>(_io_context);

	// 비동기 연결 수락
	_acceptor.async_accept(new_session->socket(),
		[this, new_session](boost::system::error_code ec) {
			if (!ec)
			{
				std::cout << "새 클라이언트 연결: "
					<< new_session->socket().remote_endpoint() << std::endl;

				// 세션 시작
				new_session->start();
			}
			else
			{
				std::cout << "Accept 오류: " << ec.message() << std::endl;
			}

			// 다음 연결을 위해 다시 accept 시작
			start_accept();
		});

}
