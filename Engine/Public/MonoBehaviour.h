#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL MonoBehaviour abstract : public Component
{
protected:
	MonoBehaviour();
public:
	virtual ~MonoBehaviour();

	virtual void Awake() override {}
	virtual void Start() override {}
	virtual void Update() override {}
	virtual void LateUpdate() override {}
	virtual void FixedUpdate() override {}

public:
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) = 0;
};

NS_END
