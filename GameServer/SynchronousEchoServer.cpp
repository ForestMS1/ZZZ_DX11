#include "pch.h"
#include "SynchronousEchoServer.h"

SynchronousEchoServer::SynchronousEchoServer(unsigned short port)
    : _acceptor(_io_context, tcp::endpoint(tcp::v4(), port))
{
    std::cout << std::format("Echo 서버가 포트 {}에서 시작되었습니다.\n", port);
}

SynchronousEchoServer::~SynchronousEchoServer()
{

}

void SynchronousEchoServer::run()
{
    try
    {
        // 무한 루프를 돌며 클라이언트 접속을 계속 기다림
        while (true)
        {
            // 1. 클라이언트 연결 대기 (블로킹)
            tcp::socket socket(_io_context);
            _acceptor.accept(socket); // 클라이언트가 접속할 때까지 여기서 멈춤
            //이 코드의 핵심적인 블로킹 부분이다. 클라이언트가 접속을 시도할 때까지 프로그램은 이 라인에서 실행을 멈추고 대기한다. 
            // 연결이 이루어지면, 해당 클라이언트와 통신할 수 있는 socket 객체가 생성된다.


            std::cout << "클라이언트가 연결되었습니다: " << socket.remote_endpoint() << std::endl;

            // 2. 연결된 클라이언트 처리
            handle_client(std::move(socket));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "서버 오류: " << e.what() << std::endl;
    }
}

// 클라이언트 통신 처리
void SynchronousEchoServer::handle_client(tcp::socket socket)
{
    try
    {
        // 한 클라이언트와 계속 통신하기 위한 루프
        while (true) 
        {
            std::array<char, 1024> buffer; // 데이터 수신용 버퍼
            buffer.fill(0);

            // 1. 데이터 수신 (블로킹)
            boost::system::error_code error;
            size_t length = socket.read_some(boost::asio::buffer(buffer), error);//또 다른 핵심 블로킹 부분이다. 클라이언트로부터 데이터가 도착할 때까지 여기서 실행을 멈추고 대기한다.

            // 2. 오류 처리 (연결 종료 등)
            if (error == boost::asio::error::eof) 
            {
                std::cout << "클라이언트가 연결을 종료했습니다." << std::endl;
                break; // 정상적인 연결 종료
            }
            else if (error) {
                throw boost::system::system_error(error); // 예외 발생
            }

            if (length > 0) 
            {
                std::string received_data(buffer.data(), length);
                std::cout << std::format("받은 메시지 [{}바이트]: {}\n",
                    length, received_data);

                std::string echo_message = std::format("Echo: {}", received_data);
                boost::system::error_code send_error;

                // 3. Echo: 받은 데이터를 그대로 전송 (블로킹)
                write(socket, boost::asio::buffer(echo_message), send_error); //수신한 데이터를 클라이언트에게 다시 보낸다. 이 작업 역시 데이터 전송이 완료될 때까지 블로킹된다.

                if (send_error) {
                    std::cout << std::format("송신 오류: {} ({})\n",
                        send_error.message(), send_error.value());
                    break;
                }

                std::cout << std::format("응답 전송: {}\n", echo_message);
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "클라이언트 처리 중 오류: " << e.what() << std::endl;
    }

    std::cout << "클라이언트 연결이 종료되었습니다.\n" << std::endl;
}
