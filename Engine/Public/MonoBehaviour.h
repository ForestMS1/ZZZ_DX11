#pragma once
#include "Component.h"

NS_BEGIN(Engine)

struct Collision;

class ENGINE_DLL MonoBehaviour abstract : public Component
{
protected:
	MonoBehaviour();
public:
	virtual ~MonoBehaviour();

	// 생명주기 함수
	virtual void Awake() override {}
	virtual void Start() override {}
	virtual void Update() override {}
	virtual void LateUpdate() override {}
	virtual void FixedUpdate() override {}

	// 충돌
	virtual void OnCollisionEnter(const Collision& collision) {};
	virtual void OnCollisionStay(const Collision& collision) {};
	virtual void OnCollisionExit(const Collision& collision) {};

	// ImGui Insperctor 창 정보 
	virtual void OnInspectorGUI() override {}

public:
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return nullptr; }
};

NS_END
