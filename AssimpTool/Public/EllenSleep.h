#pragma once
#include "BaseState.h"
class EllenSleep :
    public BaseState
{
public:
	EllenSleep(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine);
	~EllenSleep();

public:
	virtual void OnEnter() override;

	virtual void Input() override;

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;
	virtual HRESULT Render() override;


	virtual void OnExit() override;  // 상태 탈출 시 한번만 실행
};

