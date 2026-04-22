#pragma once
#include "MonoBehaviour.h"
class TestCorinScript : public MonoBehaviour
{
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnCollisionEnter(const Collision& collision) override;
	virtual void OnCollisionExit(const Collision& collision) override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return make_shared<TestCorinScript>(); }

	virtual void OnInspectorGUI() override;

public:
	static unique_ptr<TestCorinScript> Create();

private:
	weak_ptr<AnimFSM> fsm;
	uint32 _collisionCount = 0;
};

