#include "pch.h"
#include "BaseState.h"
#include "StateMachine.h"

BaseState::BaseState(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: _gameObject(gameObject), _stateMachine(stateMachine), _animator(stateMachine->_animator)
{

}

BaseState::~BaseState()
{
}

void BaseState::ToTarget()
{
	auto transform = _gameObject.lock()->GetTransform();
	auto target = _stateMachine.lock()->GetNearMonster();
	if (target != nullptr)
	{
		Vec3 targetPos = target->GetTransform()->GetPosition();
		transform->LookAtLocalY(targetPos);
	}
}