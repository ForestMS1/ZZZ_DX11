#include "pch.h"
#include "CorinEvade.h"
#include "CorinStateMachineScript.h"
#include "AnimState.h"
CorinEvade::CorinEvade(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinEvade::~CorinEvade()
{
}

void CorinEvade::OnEnter()
{
	auto animFSM = _animStateMachine.lock();
	animFSM->SetTrigger(L"evade", true);

	// TODO : 여기에 근처 몹 공격중인지 판단하는거 넣으면 될듯
}

void CorinEvade::Input()
{
	auto animFSM = _animStateMachine.lock();

	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		_stateMachine.lock()->ChangeState(L"CorinDashAttack");
	}

	const auto& desc = _gameObject.lock()->GetModelAnimator()->GetTweenDesc();
	if (GAME.Key_Pressing(DIK_UP) || GAME.Key_Pressing(DIK_DOWN))
		if (desc.curr.ratio > 0.9f)
		{
			animFSM->SetBool(L"IsMove", true);
			_stateMachine.lock()->ChangeState(L"CorinIdle");
		}
}

void CorinEvade::Awake()
{
}

void CorinEvade::Start()
{
}

void CorinEvade::Update()
{
	Input();

	if (_gameObject.lock()->GetModelAnimator()->IsCurrentAnimFinished())
		_stateMachine.lock()->ChangeState(L"CorinIdle");
}

void CorinEvade::LateUpdate()
{
}

void CorinEvade::FixedUpdate()
{
}

HRESULT CorinEvade::Render()
{
	return S_OK;
}

void CorinEvade::OnCollisionEnter(const Collision& collision)
{
}

void CorinEvade::OnCollisionExit(const Collision& collision)
{
}

void CorinEvade::OnExit()
{
}
