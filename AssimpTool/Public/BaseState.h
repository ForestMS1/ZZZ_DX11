#pragma once

class StateMachine;

class BaseState
{
public:
	BaseState(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine);
	virtual ~BaseState();

public:
	virtual void OnEnter() = 0; // 상태 진입 시 한번만 실행

	virtual void Input() = 0; // 현재 상태에서만 받을 키,마우스 입력

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	virtual HRESULT Render() { return S_OK; }

	virtual void OnCollisionEnter(const Collision& collision) {}
	virtual void OnCollisionExit(const Collision& collision) {}

	virtual void OnExit() = 0;  // 상태 탈출 시 한번만 실행

protected:
	void ToTarget();

protected:
	weak_ptr<GameObject> _gameObject;
	weak_ptr<StateMachine> _stateMachine;
	weak_ptr<ModelAnimator> _animator;
};

