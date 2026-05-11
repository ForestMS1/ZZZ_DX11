#pragma once
#include "MonoBehaviour.h"
#include "INetworkObservable.h"

class NetworkView;

class NetworkTransformView : public MonoBehaviour, INetworkObservable
{
public:
	NetworkTransformView();
	~NetworkTransformView();
public:
	virtual void Awake() override;
	virtual void Update() override;

	virtual void OnSerialize(std::vector<uint8_t>& outPayload, uint32_t& outFlags) override;
	virtual void OnDeserialize(const void* data) override;

	virtual void OnDestroy() override;

	virtual void OnInspectorGUI() override;
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return make_shared<NetworkTransformView>(*this); }

private:
	weak_ptr<NetworkView> _view;
	weak_ptr<Transform> _transformCom;

public:
	static unique_ptr<NetworkTransformView> Create();
};

