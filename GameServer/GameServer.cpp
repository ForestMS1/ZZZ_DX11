#include "pch.h"
//#include "SynchronousEchoServer.h"
#include "SimpleChatServer.h"

int main()
{
	//try
	//{
	//	SynchronousEchoServer server(PORT_NUMBER);
	//	std::cout << "Echo 서버 실행 중... (Ctrl+C로 종료)\n" << std::endl;
	//	server.run();
	//}
	//catch (std::exception& e)
	//{
	//	std::cerr << "서버 시작 실패: " << e.what() << std::endl;
	//}

	try {
		SimpleChatServer server(8080);

		// Ctrl+C 핸들러 (실제 구현에서는 signal handling 필요)
		server.run();

	}
	catch (std::exception& e) {
		std::cerr << "서버 시작 실패: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}