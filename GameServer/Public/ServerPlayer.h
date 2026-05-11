#pragma once
#include "ServerGameObject.h"

// 플레이어 클래스
class ServerPlayer : public ServerGameObject
{
public:
    ServerPlayer(uint32_t id, const std::string& username, float x, float y, float z);
    ~ServerPlayer();

public:

    // Getter/Setter 메서드들
    const std::string& GetPlayerName() const { return _userName; }
    uint32_t GetHealth() const { return _health; }
    uint32_t GetMaxHealth() const { return _maxHealth; }
    uint32_t GetZoneId() const { return _zoneId; }
    void SetZoneId(uint32_t zone_id) { _zoneId = zone_id; }

    bool IsOnline() const { return _isOnline; }
    void SetOnline(bool online) { _isOnline = online; }



    void Update(float delta_time) override {}

    std::string Serialize() const override {
        std::ostringstream oss;
        oss << "PLAYER:" << _objectId << ":" << _userName << ":"
            << _health << ":" << _maxHealth << ":"
            << _zoneId << ":" << (_isOnline ? 1 : 0);
        return oss.str();
    }

private:
    std::string _userName;
    uint32_t _health;
    uint32_t _maxHealth;
    uint32_t _zoneId;

    // 플레이어 상태
    bool _isOnline;
    Vec3 _targetPosition;
    std::chrono::steady_clock::time_point _moveStartTime;
};

