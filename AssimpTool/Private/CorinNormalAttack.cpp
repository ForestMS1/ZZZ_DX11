#include "pch.h"
#include "CorinNormalAttack.h"
#include "CorinStateMachineScript.h"
#include "AnimState.h"

CorinNormalAttack::CorinNormalAttack(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinNormalAttack::~CorinNormalAttack()
{
}

void CorinNormalAttack::OnEnter()
{
	_animator.lock()->SetTrigger(L"attackNormal");
}

void CorinNormalAttack::Input()
{
	auto animator = _animator.lock();

	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		animator->SetTrigger(L"attackNormal");
	}
}

void CorinNormalAttack::Awake()
{
}

void CorinNormalAttack::Start()
{
}

void CorinNormalAttack::Update()
{
	Input();

	if (_stateMachine.lock()->GetCurAnimStateName() == L"Idle")
		_stateMachine.lock()->ChangeState(L"CorinIdle");
}

void CorinNormalAttack::LateUpdate()
{

}

void CorinNormalAttack::FixedUpdate()
{
}

HRESULT CorinNormalAttack::Render()
{
	return S_OK;
}

void CorinNormalAttack::OnCollisionEnter(const Collision& collision)
{
}

void CorinNormalAttack::OnCollisionExit(const Collision& collision)
{
}

void CorinNormalAttack::OnExit()
{
}
