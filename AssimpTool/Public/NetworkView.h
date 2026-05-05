#pragma once
#include "MonoBehaviour.h"

class INetworkObservable;

class NetworkView : public MonoBehaviour
{
public:
    // 소유권
    bool IsMine() const { return _isMine; }
    void SetIsMine(bool mine) { _isMine = mine; }

    virtual void Awake() override;
    virtual void Update() override;

    // UUID -> uint32 (ID 충돌 가능성있음)
    uint32_t GetNetworkID() { return _networkID; }

    uint32_t GetLastSequence() const { return _lastSequence; }
    void SetLastSequence(uint32_t sequence) { _lastSequence = sequence; }

    void RegisterView(INetworkObservable* view) { _observedComponents.push_back(view); }

    virtual void OnInspectorGUI() override;

    virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return make_shared<NetworkView>(*this); }
private:
    void SerializeState();

private:
    bool _isMine = false;
    std::chrono::steady_clock::time_point _lastSendTime;
    vector<INetworkObservable*> _observedComponents; // TransformView 등이 여기 등록됨
    uint32_t _lastSequence = 0; // 게임 클라이언트를 끄지 않고 4.5년 동안 계속 위치 패킷을 쏴야 비로소 0으로 돌아감 (30fps 갱신기준)
    uint32_t _networkID = 0;
public:
    static unique_ptr<NetworkView> Create();
};

