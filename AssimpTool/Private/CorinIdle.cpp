#include "pch.h"
#include "CorinIdle.h"
#include "CorinStateMachineScript.h"

CorinIdle::CorinIdle(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinIdle::~CorinIdle()
{
}

void CorinIdle::OnEnter()
{
	auto stateMachine = static_pointer_cast<CorinStateMachineScript>(_stateMachine.lock());
	_animator.lock()->SetFloat(L"speed", stateMachine->_moveSpeed);
}

void CorinIdle::Input()
{
	auto stateMachine = static_pointer_cast<CorinStateMachineScript>(_stateMachine.lock());

	if (GAME.Key_Pressing(DIK_UP) || GAME.Key_Pressing(DIK_DOWN))
	{
		stateMachine->ChangeState(L"CorinMove");
	}

	// АјАн
	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		_stateMachine.lock()->ChangeState(L"CorinNormalAttack");
	}
}

void CorinIdle::Awake()
{
}

void CorinIdle::Start()
{
}

void CorinIdle::Update()
{
	Input();
}

void CorinIdle::LateUpdate()
{

}

void CorinIdle::FixedUpdate()
{
}

HRESULT CorinIdle::Render()
{
	return S_OK;
}

void CorinIdle::OnCollisionEnter(const Collision& collision)
{
}

void CorinIdle::OnCollisionExit(const Collision& collision)
{
}

void CorinIdle::OnExit()
{

}
