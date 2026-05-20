#pragma once
#include "BaseState.h"
class EllenIdle :
    public BaseState
{
public:
	EllenIdle(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine);
	~EllenIdle();

public:
	virtual void OnEnter() override;

	virtual void Input() override;

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;
	virtual HRESULT Render() override;

	virtual void OnCollisionEnter(const Collision& collision) override;
	virtual void OnCollisionExit(const Collision& collision) override;

	virtual void OnExit() override;  // 상태 탈출 시 한번만 실행
};

