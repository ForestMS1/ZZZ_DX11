#include "pch.h"
#include "ServerNPC.h"

ServerNPC::ServerNPC(uint32_t id, const std::string& type, float x, float y, float z)
    : ServerGameObject(id, x, y, z), _npcType(type), _health(100),
    _currentPatrolIndex(0), _isPatrolling(false) 

{
}

ServerNPC::~ServerNPC()
{
}
