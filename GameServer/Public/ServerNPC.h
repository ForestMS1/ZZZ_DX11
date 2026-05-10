#pragma once
#include "ServerGameObject.h"

// NPC Å¬·¡½º
class ServerNPC : public ServerGameObject 
{

public:
    ServerNPC(uint32_t id, const std::string& type, float x, float y, float z);
    ~ServerNPC();


private:
    std::string _npcType;
    uint32_t _health;
    std::vector<Vec3> _patrolPoints;
    size_t _currentPatrolIndex;
    bool _isPatrolling;
};