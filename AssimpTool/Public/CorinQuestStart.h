#pragma once
#include "BaseState.h"
class CorinQuestStart : public BaseState
{
public:
	CorinQuestStart(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine);
	~CorinQuestStart();

public:
	virtual void OnEnter() override;

	virtual void Input() override;

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

	virtual void OnCollisionEnter(const Collision& collision) override;
	virtual void OnCollisionExit(const Collision& collision) override;

	virtual void OnExit() override;  // 상태 탈출 시 한번만 실행
};
