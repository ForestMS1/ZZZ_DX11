#include "pch.h"
#include "EllenNormalAttack.h"
#include "EllenStateMachineScript.h"
#include "AnimState.h"

EllenNormalAttack::EllenNormalAttack(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

EllenNormalAttack::~EllenNormalAttack()
{
}

void EllenNormalAttack::OnEnter()
{
	_animator.lock()->SetTrigger(L"attackNormal");
}

void EllenNormalAttack::Input()
{
	auto animator = _animator.lock();

	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		animator->SetTrigger(L"attackNormal");
	}
}

void EllenNormalAttack::Awake()
{
}

void EllenNormalAttack::Start()
{
}

void EllenNormalAttack::Update()
{
	Input();


	const wstring& curAnimName = _stateMachine.lock()->GetCurAnimStateName();
	if (curAnimName == L"Idle" || curAnimName == L"Walk_Start")
		_stateMachine.lock()->ChangeState(L"EllenIdle");
}

void EllenNormalAttack::LateUpdate()
{

}

void EllenNormalAttack::FixedUpdate()
{
}

HRESULT EllenNormalAttack::Render()
{
	return S_OK;
}

void EllenNormalAttack::OnCollisionEnter(const Collision& collision)
{
}

void EllenNormalAttack::OnCollisionExit(const Collision& collision)
{
}

void EllenNormalAttack::OnExit()
{
}
