#include "pch.h"
#include "EllenIdle.h"
#include "EllenStateMachineScript.h"

EllenIdle::EllenIdle(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

EllenIdle::~EllenIdle()
{
}

void EllenIdle::OnEnter()
{
	auto stateMachine = static_pointer_cast<EllenStateMachineScript>(_stateMachine.lock());
	_animator.lock()->SetFloat(L"speed", stateMachine->_moveSpeed);
}

void EllenIdle::Input()
{
	auto stateMachine = static_pointer_cast<EllenStateMachineScript>(_stateMachine.lock());

	if (GAME.Key_Pressing(DIK_UP) || GAME.Key_Pressing(DIK_DOWN))
	{
		stateMachine->ChangeState(L"EllenMove");
	}

	// АјАн
	if (GAME.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
	{
		_stateMachine.lock()->ChangeState(L"EllenNormalAttack");
	}
}

void EllenIdle::Awake()
{
}

void EllenIdle::Start()
{
}

void EllenIdle::Update()
{
	Input();
}

void EllenIdle::LateUpdate()
{

}

void EllenIdle::FixedUpdate()
{
}

HRESULT EllenIdle::Render()
{
	return S_OK;
}

void EllenIdle::OnCollisionEnter(const Collision& collision)
{
}

void EllenIdle::OnCollisionExit(const Collision& collision)
{
}

void EllenIdle::OnExit()
{

}
