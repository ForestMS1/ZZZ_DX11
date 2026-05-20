#include "pch.h"
#include "EllenEvade.h"
#include "EllenStateMachineScript.h"
#include "AnimState.h"
EllenEvade::EllenEvade(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

EllenEvade::~EllenEvade()
{
}

void EllenEvade::OnEnter()
{
	auto animator = _animator.lock();


	animator->SetTrigger(L"evade");
	if (GAME.Key_Pressing(DIK_DOWN))
		animator->SetBool(L"isFront", false);
	else
		animator->SetBool(L"isFront", true);

	// TODO : 여기에 근처 몹 공격중인지 판단하는거 넣으면 될듯
}

void EllenEvade::Input()
{

}

void EllenEvade::Awake()
{
}

void EllenEvade::Start()
{
}

void EllenEvade::Update()
{
	Input();

	const auto& stateMachine = _stateMachine.lock();
	if (stateMachine->GetCurAnimStateName() == L"Idle")
		_stateMachine.lock()->ChangeState(L"EllenIdle");
}

void EllenEvade::LateUpdate()
{
}

void EllenEvade::FixedUpdate()
{
}

HRESULT EllenEvade::Render()
{
	return S_OK;
}

void EllenEvade::OnCollisionEnter(const Collision& collision)
{
}

void EllenEvade::OnCollisionExit(const Collision& collision)
{
}

void EllenEvade::OnExit()
{

}
