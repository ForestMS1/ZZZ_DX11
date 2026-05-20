#pragma once
#include "StateMachine.h"

class BaseState;

// 엘렌의 모든 로직이 들어갈 스크립트다. (싱글모드)
class EllenStateMachineScript : public StateMachine
{
public:
	EllenStateMachineScript();
	EllenStateMachineScript(const EllenStateMachineScript& rhs);
	~EllenStateMachineScript();
public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;
	virtual HRESULT Render() override;

	virtual void OnCollisionEnter(const Collision& collision) override;
	virtual void OnCollisionExit(const Collision& collision) override;
	virtual void OnDestroy() override;


	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { auto script = make_shared<EllenStateMachineScript>(*this); script->Initialize();  return script; }
	virtual void OnInspectorGUI() override;
public:
	static unique_ptr<EllenStateMachineScript> Create();

	// 이벤트 함수
private:
	void OnQuestStart();

	// 게임데이터 (hp, speed같은 값)
private:
	friend class EllenIdle;
	friend class EllenNormalAttack;
	friend class EllenEvade;
	friend class EllenDashAttack;

	float _maxHp = 1000.f;
	float _hp = _maxHp;

	float _moveSpeedMax = 5.f;
	float _moveSpeed = 0.f;
	float _moveAccel = 10.f;
};

