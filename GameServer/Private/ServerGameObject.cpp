#include "pch.h"
#include "ServerGameObject.h"

ServerGameObject::ServerGameObject(uint32_t id, float x, float y, float z)
	: _objectId(id), _position(x,y,z), _rotation(0.f,0.f,0.f),
	_lastUpdate(std::chrono::steady_clock::now())
{
}

ServerGameObject::~ServerGameObject()
{
}