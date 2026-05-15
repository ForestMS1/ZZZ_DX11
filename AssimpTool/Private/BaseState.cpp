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
