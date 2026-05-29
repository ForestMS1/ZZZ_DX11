#include "pch.h"
#include "CorinSwitchOut.h"
#include "CorinStateMachineScript.h"
#include "AnimState.h"

CorinSwitchOut::CorinSwitchOut(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinSwitchOut::~CorinSwitchOut()
{
}

void CorinSwitchOut::OnEnter()
{
	auto animator = _animator.lock();
	animator->SetTrigger(L"switchOut");

	animator->SetCurRenderGroup(RENDERGROUP::BLEND);
	animator->SetPass(2);

	_alphaValue = 1.f;

	auto collider = _gameObject.lock()->GetCollider();
	collider->SetActive(false);
}

void CorinSwitchOut::Input()
{

}

void CorinSwitchOut::Awake()
{
}

void CorinSwitchOut::Start()
{
}

void CorinSwitchOut::Update()
{
	_alphaValue -= _fadeSpeed * DT;
	if (_alphaValue < 0.f)
	{
		_stateMachine.lock()->ChangeState(L"Sleep");
		_alphaValue = 0.f;
	}

	auto shader = _animator.lock()->GetShader();
	shader->GetScalar("g_AlphaValue")->SetFloat(_alphaValue);

	// TODO : 애니메이션이 끝났다면 Sleep상태로 전환
	//if (_animator.lock()->IsCurrentAnimFinished())
	//{
	//	_stateMachine.lock()->ChangeState(L"Sleep");
	//}
}

void CorinSwitchOut::LateUpdate()
{

}

void CorinSwitchOut::FixedUpdate()
{
}

HRESULT CorinSwitchOut::Render()
{

	return S_OK;
}

void CorinSwitchOut::OnCollisionEnter(const Collision& collision)
{
}

void CorinSwitchOut::OnCollisionExit(const Collision& collision)
{
}

void CorinSwitchOut::OnExit()
{

}
