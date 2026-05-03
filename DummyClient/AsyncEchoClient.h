#pragma once
class AsyncEchoClient : public std::enable_shared_from_this<AsyncEchoClient>
{
public:
	AsyncEchoClient(io_context& io_context);
	~AsyncEchoClient();

public:
	void start(const string& host, unsigned short port);
	void sendMessage(const string& message);
	void close();

private:
	void do_connect(tcp::resolver::results_type endpoints);
	void do_read();
	void do_write();

private:
	io_context& _io_context;
	tcp::socket _socket;

	array<char, 1024> _receiveBuffer;
	string _sendBuffer;

	queue<string> _messageQueue;

	bool _writing;
};

