#include "pch.h"
#include "GameUDPServer.h"
int main()
{
	//try {
	//	SimpleChatServer server(8080);

	//	// Ctrl+C 핸들러 (실제 구현에서는 signal handling 필요)
	//	server.run();

	//}
	//catch (std::exception& e) {
	//	std::cerr << "서버 시작 실패: " << e.what() << std::endl;
	//	return 1;
	//}

	//try
	//{
	//	io_context io_context;

	//	AsyncEchoServer server(io_context, 8080);

	//	std::cout << "비동기 Echo 서버 실행 중...\n" << std::endl;
	//	io_context.run();
	//}
	//catch (std::exception& e)
	//{
	//	std::cerr << "서버 오류: " << e.what() << std::endl;
	//}

	try
	{
		boost::asio::io_context io_context;

		auto server = make_shared<GameUDPServer>(io_context, 9000);
		std::cout << "Game UDP 서버 실행 중...\n" << std::endl;
		
		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "서버 오류: " << e.what() << std::endl;
	}



	return 0;
}