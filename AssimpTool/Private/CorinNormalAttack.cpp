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
	_animator.lock()->SetBool(L"attackNormal", true);
	_animator.lock()->SetFloat(L"attackNormalStep", 0.f);
}

void CorinNormalAttack::Input()
{
	auto animator = _animator.lock();

	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		if (animator->GetFloat(L"attackNormalStep") == 0.f)
		{
			animator->SetFloat(L"attackNormalStep", 1.f);
		}
		else if (animator->GetFloat(L"attackNormalStep") == 1.f)
		{
			animator->SetFloat(L"attackNormalStep", 2.f);
		}
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

	if (_gameObject.lock()->GetModelAnimator()->IsCurrentAnimFinished())
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
	_animator.lock()->SetBool(L"attackNormal", false);
	_animator.lock()->SetFloat(L"attackNormalStep", 0.f);
}
