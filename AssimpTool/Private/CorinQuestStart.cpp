#include "pch.h"
#include "CorinQuestStart.h"
#include "CorinStateMachineScript.h"

CorinQuestStart::CorinQuestStart(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinQuestStart::~CorinQuestStart()
{
}

void CorinQuestStart::OnEnter()
{
	_animator.lock()->SetTrigger(L"questStart");
}

void CorinQuestStart::Input()
{

}

void CorinQuestStart::Awake()
{
}

void CorinQuestStart::Start()
{
}

void CorinQuestStart::Update()
{
	const auto& stateMachine = _stateMachine.lock();
	if (stateMachine->GetCurAnimStateName() == L"Idle")
		_stateMachine.lock()->ChangeState(L"CorinIdle");
}

void CorinQuestStart::LateUpdate()
{

}

void CorinQuestStart::FixedUpdate()
{
}



void CorinQuestStart::OnCollisionEnter(const Collision& collision)
{
}

void CorinQuestStart::OnCollisionExit(const Collision& collision)
{
}

void CorinQuestStart::OnExit()
{
	shared_ptr<EventDesc> eventDesc = make_shared<EventDesc>();
	eventDesc->eventType = static_cast<uint32>(EventType::QUEST_START_END);
	eventDesc->sender = this;
	GAME.PushEvent(eventDesc);
}
