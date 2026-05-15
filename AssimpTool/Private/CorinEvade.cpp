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
	auto animator = _animator.lock();

	//auto stateMachine = static_pointer_cast<CorinStateMachineScript>(_stateMachine.lock());
	//stateMachine->_moveSpeed = stateMachine->_moveSpeedMax;
	//animator->SetFloat(L"speed", stateMachine->_moveSpeed);

	animator->SetTrigger(L"evade");
	if (GAME.Key_Pressing(DIK_DOWN))
		animator->SetBool(L"isFront", false);
	else
		animator->SetBool(L"isFront", true);

	// TODO : 여기에 근처 몹 공격중인지 판단하는거 넣으면 될듯
}

void CorinEvade::Input()
{
	const auto& animator = _gameObject.lock()->GetModelAnimator();
	const auto& stateMachine = _stateMachine.lock();

	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		_stateMachine.lock()->ChangeState(L"CorinDashAttack");
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

	const auto& stateMachine = _stateMachine.lock();
	if (stateMachine->GetCurAnimStateName() == L"Idle")
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
	_animator.lock()->SetBool(L"evadeToRun", false);
}
