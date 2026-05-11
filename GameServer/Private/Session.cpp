#include "pch.h"
#include "Session.h"
#include "ServerPlayer.h"

Session::Session(udp::endpoint endpoint, uint32_t sessionId)
	: _endpoint(endpoint), _sessionId(sessionId), 
	_lastSeen(std::chrono::steady_clock::now())
{
}

Session::~Session()
{
}

uint32_t Session::GetPlayerID() const
{ 
	return (_player != nullptr) ? _player->GetId() : 0; 
}

void Session::SetPlayer(shared_ptr<ServerPlayer> player)
{
	if (_player)
		_player->SetOwnerSessionId(0);

	_player = player;
	_player->SetOwnerSessionId(_sessionId);
}

bool Session::IsSequenceValid(uint32_t newSequence) 
{
	if (newSequence > _lastSequence) {
		_lastSequence = newSequence;
		return true;
	}
	return false;
}

void Session::Send(udp::socket& socket, const string& packetData) {
	socket.async_send_to(boost::asio::buffer(packetData), _endpoint,
		[](const boost::system::error_code& ec, std::size_t bytes) {
			// 에러 처리
		});
}