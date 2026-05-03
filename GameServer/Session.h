#pragma once
class Session : public enable_shared_from_this<Session>
{
public:
	Session(io_context& io_context);
	~Session();

public:
	
	tcp::socket& socket() { return _socket; }

	void start();


private:
	void do_read();
	void do_write(size_t length);
	string get_client_info();
private:
	tcp::socket _socket;
	array<char, 1024> _buffer;
};

