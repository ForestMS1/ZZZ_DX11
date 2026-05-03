#pragma once
class AsyncEchoServer
{
public:
	AsyncEchoServer(io_context& io_context, unsigned short port);
	~AsyncEchoServer();

public:


private:
	void start_accept();

private:

	io_context& _io_context;
	tcp::acceptor _acceptor;
};

