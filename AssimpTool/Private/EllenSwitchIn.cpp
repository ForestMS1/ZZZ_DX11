#include "pch.h"
#include "EllenSwitchIn.h"
#include "EllenStateMachineScript.h"
#include "AnimState.h"

EllenSwitchIn::EllenSwitchIn(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

EllenSwitchIn::~EllenSwitchIn()
{
}

void EllenSwitchIn::OnEnter()
{
	auto animator = _animator.lock();
	animator->SetTrigger(L"switchIn");

	animator->SetCurRenderGroup(RENDERGROUP::NONBLEND);
	animator->SetPass(0);

	// 조건문 잡고 
	// switchInAttack으로도 가능하게
}

void EllenSwitchIn::Input()
{

}

void EllenSwitchIn::Awake()
{
}

void EllenSwitchIn::Start()
{
}

void EllenSwitchIn::Update()
{
	const wstring& curAnimName = _stateMachine.lock()->GetCurAnimStateName();
	if (curAnimName == L"Idle" || curAnimName == L"Walk_Start")
		_stateMachine.lock()->ChangeState(L"EllenIdle");
}

void EllenSwitchIn::LateUpdate()
{

}

void EllenSwitchIn::FixedUpdate()
{
}

HRESULT EllenSwitchIn::Render()
{
	return S_OK;
}

void EllenSwitchIn::OnCollisionEnter(const Collision& collision)
{
}

void EllenSwitchIn::OnCollisionExit(const Collision& collision)
{
}

void EllenSwitchIn::OnExit()
{
}
