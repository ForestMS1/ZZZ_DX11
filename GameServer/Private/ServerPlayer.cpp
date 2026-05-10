#include "pch.h"
#include "ServerPlayer.h"

ServerPlayer::ServerPlayer(uint32_t id, const std::string& username, float x, float y, float z)
    : ServerGameObject(id, x, y, z), _userName(username),
    _health(100), _maxHealth(100),
    _zoneId(0), _isOnline(true) 
{
}

ServerPlayer::~ServerPlayer()
{
}
