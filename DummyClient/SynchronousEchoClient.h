#pragma once

const char SERVER_IP[] = "127.0.0.1";
const unsigned short PORT_NUMBER = 8080;

class SynchronousEchoClient
{
public:
    SynchronousEchoClient();
    ~SynchronousEchoClient();

public:
    bool connect_to_server(const std::string& server_ip, unsigned short port);

    void run_interactive_session();

    void run_automatic_test();

    void disconnect();

private:
    bool send_and_receive_message(const std::string& message);

private:
    io_context _io_context; //서버와 마찬가지로 Asio의 핵심 객체이다.
    tcp::socket _socket; //서버와 실제로 데이터를 주고받는 통신 채널 역할을 하는 객체이다.
};
