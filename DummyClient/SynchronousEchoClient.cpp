#include "pch.h"
#include "SynchronousEchoClient.h"

SynchronousEchoClient::SynchronousEchoClient()
	: _socket(_io_context) // _io_context를 사용하여 _socket을 초기화한다.
{
}

SynchronousEchoClient::~SynchronousEchoClient()
{
}

bool SynchronousEchoClient::connect_to_server(const std::string& server_ip, unsigned short port)
{
    try
    {
        // 서버 주소 설정
        tcp::endpoint server_endpoint(
            boost::asio::ip::make_address(server_ip), port);

        // 서버에 연결 (블로킹)
        boost::system::error_code connect_error;
        _socket.connect(server_endpoint, connect_error);

        if (connect_error)
        {
            std::cout << std::format("연결 실패: {} ({})\n",
                connect_error.message(), connect_error.value());
            return false;
        }

        std::cout << "서버에 성공적으로 연결되었습니다: "
            << server_endpoint << std::endl;
        return true;

    }
    catch (std::exception& e)
    {
        std::cerr << "연결 중 예외 발생: " << e.what() << std::endl;
        return false;
    }
}

void SynchronousEchoClient::run_interactive_session()
{
    std::cout << "\n=== Echo 클라이언트 시작 ===\n";
    std::cout << "메시지를 입력하세요 ('quit' 입력 시 종료):\n" << std::endl;

    std::string input;
    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "quit" || input == "exit") {
            std::cout << "클라이언트를 종료합니다." << std::endl;
            break;
        }

        if (input.empty()) {
            continue;
        }

        if (!send_and_receive_message(input)) {
            std::cout << "통신 오류로 인해 연결을 종료합니다." << std::endl;
            break;
        }
    }
}

void SynchronousEchoClient::run_automatic_test()
{
    std::cout << "\n=== 자동 테스트 모드 ===\n" << std::endl;

    std::vector<std::string> test_messages = {
        "Hello, Server!",
        "Test message 1",
        "게임 서버 테스트",
        "123456789",
        "Special chars: !@#$%^&*()"
    };

    for (size_t i = 0; i < test_messages.size(); ++i)
    {
        std::cout << std::format("테스트 {}/{}: {}\n",
            i + 1, test_messages.size(), test_messages[i]);

        if (!send_and_receive_message(test_messages[i]))
        {
            std::cout << "테스트 실패!" << std::endl;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "자동 테스트 완료!" << std::endl;
}

void SynchronousEchoClient::disconnect()
{
    if (_socket.is_open()) {
        boost::system::error_code error;
        _socket.close(error);

        if (!error) {
            std::cout << "서버와의 연결이 종료되었습니다." << std::endl;
        }
    }
}

bool SynchronousEchoClient::send_and_receive_message(const std::string& message)
{
    try
    {
        // 메시지 전송 (블로킹)
        boost::system::error_code send_error;
        write(_socket, boost::asio::buffer(message), send_error);

        if (send_error)
        {
            std::cout << std::format("전송 오류: {} ({})\n",
                send_error.message(), send_error.value());
            return false;
        }

        std::cout << std::format("전송: {}\n", message);

        // 응답 수신 (블로킹)
        std::array<char, 1024> buffer;
        buffer.fill(0);

        boost::system::error_code receive_error;
        size_t reply_length = _socket.read_some(
            boost::asio::buffer(buffer), receive_error);

        if (receive_error == boost::asio::error::eof) {
            std::cout << "서버가 연결을 종료했습니다." << std::endl;
            return false;
        }
        else if (receive_error) {
            std::cout << std::format("수신 오류: {} ({})\n",
                receive_error.message(), receive_error.value());
            return false;
        }

        std::string reply(buffer.data(), reply_length);
        std::cout << std::format("수신: {}\n", reply);
        std::cout << std::endl;

        return true;

    }
    catch (std::exception& e)
    {
        std::cerr << "통신 중 예외 발생: " << e.what() << std::endl;
        return false;
    }
}
