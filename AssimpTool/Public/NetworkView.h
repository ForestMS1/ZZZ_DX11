#pragma once
#include "MonoBehaviour.h"

#include "INetworkObservable.h"

class NetworkView : public MonoBehaviour
{
public:
    // 소유권
    bool IsMine() const { return _isMine; }
    void SetIsMine(bool mine) { _isMine = mine; }

    virtual HRESULT Initialize_Prototype() override;

    virtual void Awake() override;
    virtual void Update() override;

    // UUID -> uint32 (ID 충돌 가능성있음, 소규모라 확률 매우 희박, 일단 구현부터)
    uint32_t GetNetworkID() { return _networkID; }

    uint32_t GetLastSequence() const { return _lastSequence; }
    void SetLastSequence(uint32_t sequence) { _lastSequence = sequence; }

    // INetworkObservable 쪽에서 호출
    void RegisterView(INetworkObservable* view) { _observedComponents[static_cast<uint8>(view->GetViewType())] = view; }
    void RemoveView(NetworkViewType type) { _observedComponents[static_cast<uint8>(type)] = nullptr; }

    virtual void OnInspectorGUI() override;

    virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return make_shared<NetworkView>(*this); }
private:
    // INetworkObservable의 데이터들을 모아서 하나의 패킷으로 전송
    void SerializeState();

private:
    bool _isMine = false;
    std::chrono::steady_clock::time_point _lastSendTime;
    array<INetworkObservable*, VIEW_COUNT> _observedComponents; // TransformView, AnimationView 등이 여기 등록됨 // 순회 순서 보장하기 위해 array
    //vector<INetworkObservable*> _observedComponents; // TransformView, AnimationView 등이 여기 등록됨
    uint32_t _lastSequence = 0; // 게임 클라이언트를 끄지 않고 4.5년 동안 계속 위치 패킷을 쏴야 비로소 0으로 돌아감 (30fps 갱신기준)
    uint32_t _networkID = 0;
public:
    static unique_ptr<NetworkView> Create();
};

