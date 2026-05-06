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

		// 작업이 없어도 run()이 종료되지 않도록 유지
		auto workGuard = boost::asio::make_work_guard(io_context);

		auto server = make_shared<GameUDPServer>(io_context, 9000);
		std::cout << "Game UDP 서버 실행 중...\n" << std::endl;
		


		std::vector<std::thread> workers;

		for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
		{
			workers.emplace_back([&]()
				{
					io_context.run();
				});
		}

		for (auto& t : workers)
			t.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "서버 오류: " << e.what() << std::endl;
	}



	return 0;
}