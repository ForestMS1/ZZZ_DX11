#pragma once
#include "MonoBehaviour.h"

class NetworkView;

class TestCorinScript : public MonoBehaviour
{
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnCollisionEnter(const Collision& collision) override;
	virtual void OnCollisionExit(const Collision& collision) override;

	virtual void OnDestroy() override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return make_shared<TestCorinScript>(*this); }

	virtual void OnInspectorGUI() override;

public:
	static unique_ptr<TestCorinScript> Create();

private:
	weak_ptr<NetworkView> _view = {};
	weak_ptr<AnimFSM> _fsm = {};
	uint32 _collisionCount = 0;

private:
	float _moveSpeed = 0.f;
};

