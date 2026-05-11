#pragma once

// 게임 오브젝트 기본 클래스
class ServerGameObject 
{
public:
    ServerGameObject(uint32_t id, float x, float y, float z);
    virtual ~ServerGameObject();

public:
    uint32_t GetId() const { return _objectId; }
    void SetId(uint32_t id) { _objectId = id; }

    string GetFactoryName() { return _className; }
    void SetFactoryName(const string& className) { _className = className; }

    uint32_t GetOwnerSessionId() const { return _ownerSessionId; }
    void SetOwnerSessionId(uint32_t id) { _ownerSessionId = id; }

    Vec3 GetPosition() { return _position; }

    virtual void SetPosition(float x, float y, float z) 
    {
        _position.x = x;
        _position.y = y;
        _position.z = z;
        _lastUpdate = std::chrono::steady_clock::now();
    }

    void SetRotation(float x, float y, float z) {
        _rotation.x = x;
        _rotation.y = y;
        _rotation.z = z;
        _lastUpdate = std::chrono::steady_clock::now();
    }

    virtual std::string Serialize() const = 0;
    virtual void Update(float delta_time) {}

    std::chrono::steady_clock::time_point GetLastUpdate() const { return _lastUpdate; }

protected:
    uint32_t _objectId;
    uint32_t _ownerSessionId; // 소유주 판별용

    string _className;

    Vec3 _scale;
    Vec3 _rotation;
    Vec3 _position;

    std::chrono::steady_clock::time_point _lastUpdate;
};