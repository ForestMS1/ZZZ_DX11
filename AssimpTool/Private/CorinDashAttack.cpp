#include "pch.h"
#include "CorinDashAttack.h"
#include "CorinStateMachineScript.h"
#include "AnimState.h"

CorinDashAttack::CorinDashAttack(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinDashAttack::~CorinDashAttack()
{
}

void CorinDashAttack::OnEnter()
{
	_animator.lock()->SetTrigger(L"attackRush", true);
}

void CorinDashAttack::Input()
{
	
}

void CorinDashAttack::Awake()
{
}

void CorinDashAttack::Start()
{
}

void CorinDashAttack::Update()
{
	Input();


	if (_gameObject.lock()->GetModelAnimator()->IsCurrentAnimFinished())
		_stateMachine.lock()->ChangeState(L"CorinIdle");
}

void CorinDashAttack::LateUpdate()
{
}

void CorinDashAttack::FixedUpdate()
{
}

HRESULT CorinDashAttack::Render()
{
	return S_OK;
}

void CorinDashAttack::OnCollisionEnter(const Collision& collision)
{
}

void CorinDashAttack::OnCollisionExit(const Collision& collision)
{
}

void CorinDashAttack::OnExit()
{

}
