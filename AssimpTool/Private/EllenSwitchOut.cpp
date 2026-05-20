#include "pch.h"
#include "EllenSwitchOut.h"
#include "EllenStateMachineScript.h"
#include "AnimState.h"

EllenSwitchOut::EllenSwitchOut(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

EllenSwitchOut::~EllenSwitchOut()
{
}

void EllenSwitchOut::OnEnter()
{
	auto animator = _animator.lock();
	animator->SetTrigger(L"switchOut");

	//animator->SetCurRenderGroup(RENDERGROUP::BLEND);
	//animator->SetPass(2);
}

void EllenSwitchOut::Input()
{

}

void EllenSwitchOut::Awake()
{
}

void EllenSwitchOut::Start()
{
}

void EllenSwitchOut::Update()
{
	// TODO : 애니메이션이 끝났다면 Sleep상태로 전환
	if (_animator.lock()->IsCurrentAnimFinished())
	{
		_stateMachine.lock()->ChangeState(L"Sleep");
	}
}

void EllenSwitchOut::LateUpdate()
{

}

void EllenSwitchOut::FixedUpdate()
{
}

HRESULT EllenSwitchOut::Render()
{
	return S_OK;
}

void EllenSwitchOut::OnCollisionEnter(const Collision& collision)
{
}

void EllenSwitchOut::OnCollisionExit(const Collision& collision)
{
}

void EllenSwitchOut::OnExit()
{
}
