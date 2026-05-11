#include "pch.h"
//#include "SynchronousEchoClient.h"
#include "AsyncEchoClient.h"
//프로그램의 시작점이다.
//argc, argv는 프로그램을 실행할 때 전달하는 인자이다.예를 들어, 터미널에서 my_client.exe --auto와 같이 실행하면, --auto라는 인자를 감지하여 auto_mode를 활성화한다.
//프로그램은 연결->통신->연결 종료의 명확한 순서로 동작한다.
#include "GameUDPClient.h"

int main(int argc, char* argv[])
{
    //try
    //{
    //    SynchronousEchoClient client;

    //    // 서버에 연결
    //    if (!client.connect_to_server(SERVER_IP, PORT_NUMBER)) {
    //        return 1;
    //    }

    //    // 실행 모드 선택
    //    bool auto_mode = false;
    //    if (argc > 1 && std::string(argv[1]) == "--auto") {
    //        auto_mode = true;
    //    }

    //    if (auto_mode) {
    //        client.run_automatic_test();
    //    }
    //    else {
    //        client.run_interactive_session();
    //    }

    //    client.disconnect();

    //}
    //catch (std::exception& e)
    //{
    //    std::cerr << "클라이언트 오류: " << e.what() << std::endl;
    //    return 1;
    //}


    try
    {
        io_context io_context;

        auto client = make_shared<GameUDPClient>(io_context, "127.0.0.1", "9000");

        client->SendPlayerPosition(0, 0, 0, 1, 1, 1);
        

        io_context.run();

        //io_context.stop();
    }
    catch (std::exception& e) {
        std::cerr << "클라이언트 오류: " << e.what() << std::endl;
    }

    return 0;
}