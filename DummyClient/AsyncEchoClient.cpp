#include "pch.h"
#include "AsyncEchoClient.h"

AsyncEchoClient::AsyncEchoClient(io_context& io_context)
	: _io_context(io_context)
	, _socket(io_context)
	, _writing(false)
{
}

AsyncEchoClient::~AsyncEchoClient()
{
}

void AsyncEchoClient::start(const string& host, unsigned short port)
{
    // 서버 주소 해석
    auto resolver = std::make_shared<tcp::resolver>(_io_context);

    resolver->async_resolve(host, std::to_string(port),
        [this, resolver](boost::system::error_code ec,
            tcp::resolver::results_type endpoints) {
                if (!ec) {
                    do_connect(endpoints);
                }
                else {
                    std::cout << "주소 해석 실패: " << ec.message() << std::endl;
                }
        });
}

void AsyncEchoClient::sendMessage(const string& message)
{
    boost::asio::post(_io_context,
        [this, self = shared_from_this(), message]() {
            bool write_in_progress = !_messageQueue.empty();
            _messageQueue.push(message);

            if (!write_in_progress && _socket.is_open()) {
                do_write();
            }
        });
}

void AsyncEchoClient::close()
{
    boost::asio::post(_io_context,
        [this, self = shared_from_this()]() {
            if (_socket.is_open()) {
                _socket.close();
            }
        });
}

void AsyncEchoClient::do_connect(tcp::resolver::results_type endpoints)
{
    boost::asio::async_connect(_socket, endpoints,
        [this, self = shared_from_this()](boost::system::error_code ec,
            tcp::endpoint) {
                if (!ec) {
                    std::cout << "서버 연결 성공!" << std::endl;
                    do_read();
                }
                else {
                    std::cout << "연결 실패: " << ec.message() << std::endl;
                }
        });
}

void AsyncEchoClient::do_read()
{
    _socket.async_read_some(boost::asio::buffer(_receiveBuffer),
        [this, self = shared_from_this()](boost::system::error_code ec,
            std::size_t length) {
                if (!ec) {
                    std::string received_data(_receiveBuffer.data(), length);
                    std::cout << std::format("서버 응답: {}\n", received_data);

                    // 계속해서 읽기
                    do_read();
                }
                else if (ec == boost::asio::error::eof) {
                    std::cout << "서버가 연결을 종료했습니다." << std::endl;
                }
                else {
                    std::cout << "읽기 오류: " << ec.message() << std::endl;
                }
        });
}

void AsyncEchoClient::do_write()
{
    if (_messageQueue.empty()) {
        return;
    }

    _sendBuffer = _messageQueue.front();

    boost::asio::async_write(_socket, boost::asio::buffer(_sendBuffer),
        [this, self = shared_from_this()](boost::system::error_code ec,
            std::size_t bytes_written) {
                if (!ec) {
                    std::cout << std::format("메시지 전송 완료: {} bytes\n",
                        bytes_written);

                    _messageQueue.pop();

                    // 대기 중인 메시지가 더 있으면 계속 전송
                    if (!_messageQueue.empty()) {
                        do_write();
                    }
                }
                else {
                    std::cout << "쓰기 오류: " << ec.message() << std::endl;
                }
        });
}