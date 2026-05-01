#pragma once


const char SERVER_IP[] = "127.0.0.1";
const unsigned short PORT_NUMBER = 31400;

class SynchronousEchoServer
{
public:
    SynchronousEchoServer(unsigned short port);
    ~SynchronousEchoServer();

    void run();

private:
    void handle_client(tcp::socket socket);
private:
    //Boost.Asio의 심장과 같은 객체이다.
    // 운영체제의 I / O(입출력) 서비스와 상호작용하는 역할을 하며, 모든 I / O 작업의 실행 컨텍스트를 제공한다.
    io_context _io_context;


    // 클라이언트의 연결 요청을 수락하는 "문지기" 역할을 한다. 지정된 포트에서 클라이언트의 접속을 기다린다.
    tcp::acceptor _acceptor;
};

