#pragma once
#include "BaseState.h"
class CorinSwitchOut :
    public BaseState
{
public:
	CorinSwitchOut(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine);
	~CorinSwitchOut();

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

private:
	float _alphaValue = 1.f;
	float _fadeSpeed = 1.f;
};

