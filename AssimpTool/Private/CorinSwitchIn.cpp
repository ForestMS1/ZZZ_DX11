#include "pch.h"
#include "CorinSwitchIn.h"
#include "CorinStateMachineScript.h"
#include "AnimState.h"

CorinSwitchIn::CorinSwitchIn(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinSwitchIn::~CorinSwitchIn()
{
}

void CorinSwitchIn::OnEnter()
{
	auto animator = _animator.lock();
	animator->SetTrigger(L"switchInAttack");

	//animator->SetCurRenderGroup(RENDERGROUP::NONBLEND);
	//animator->SetPass(1);

	// 조건문 잡고 
	// switchInAttack으로도 가능하게
}

void CorinSwitchIn::Input()
{

}

void CorinSwitchIn::Awake()
{
}

void CorinSwitchIn::Start()
{
}

void CorinSwitchIn::Update()
{
	const auto& stateMachine = _stateMachine.lock();
	if (stateMachine->GetCurAnimStateName() == L"Idle")
		_stateMachine.lock()->ChangeState(L"CorinIdle");
}

void CorinSwitchIn::LateUpdate()
{

}

void CorinSwitchIn::FixedUpdate()
{
}

HRESULT CorinSwitchIn::Render()
{
	return S_OK;
}

void CorinSwitchIn::OnCollisionEnter(const Collision& collision)
{
}

void CorinSwitchIn::OnCollisionExit(const Collision& collision)
{
}

void CorinSwitchIn::OnExit()
{
}
