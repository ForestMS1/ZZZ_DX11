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
	auto animFSM = _animStateMachine.lock();
	animFSM->SetBool(L"attackNormal", true);
	animFSM->SetFloat(L"attackNormalStep", 0.f);
}

void CorinNormalAttack::Input()
{
	auto animFSM = _animStateMachine.lock();

	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		if (animFSM->GetFloat(L"attackNormalStep") == 0.f)
		{
			animFSM->SetFloat(L"attackNormalStep", 1.f);
		}
		else if (animFSM->GetFloat(L"attackNormalStep") == 1.f)
		{
			animFSM->SetFloat(L"attackNormalStep", 2.f);
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
	auto animFSM = _animStateMachine.lock();
	animFSM->SetBool(L"attackNormal", false);
	animFSM->SetFloat(L"attackNormalStep", 0.f);
}
