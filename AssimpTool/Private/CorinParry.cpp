#include "pch.h"
#include "CorinParry.h"
#include "CorinStateMachineScript.h"
#include "AnimState.h"
#include "ParticleSystem.h"
#include "MonsterScript.h"

CorinParry::CorinParry(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinParry::~CorinParry()
{
}

void CorinParry::OnEnter()
{
	auto animator = _animator.lock();
	animator->SetTrigger(L"ParryAid");

	animator->SetCurRenderGroup(RENDERGROUP::NONBLEND);
	animator->SetPass(0);

	auto shader = _animator.lock()->GetShader();
	shader->GetScalar("g_AlphaValue")->SetFloat(1.f);

	auto collider = _gameObject.lock()->GetCollider();
	collider->SetActive(true);


	// 패링 이벤트
	shared_ptr<EventDesc> eventDesc = make_shared<EventDesc>();
	eventDesc->eventType = static_cast<uint32>(EventType::PARRY);
	eventDesc->sender = this;
	GAME.PushEvent(eventDesc);
}

void CorinParry::Input()
{

}

void CorinParry::Awake()
{
}

void CorinParry::Start()
{
}

void CorinParry::Update()
{
	const auto& stateMachine = _stateMachine.lock();
	if (stateMachine->GetCurAnimStateName() == L"Idle")
		_stateMachine.lock()->ChangeState(L"CorinIdle");
}

void CorinParry::LateUpdate()
{

}

void CorinParry::FixedUpdate()
{
}

HRESULT CorinParry::Render()
{
	return S_OK;
}

void CorinParry::OnCollisionEnter(const Collision& collision)
{
}

void CorinParry::OnCollisionExit(const Collision& collision)
{
}

void CorinParry::OnExit()
{
}
