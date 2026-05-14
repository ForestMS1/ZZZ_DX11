#include "pch.h"
#include "StateMachine.h"
#include "BaseState.h"

StateMachine::StateMachine()
{
}

StateMachine::StateMachine(const StateMachine& rhs)
	: MonoBehaviour(rhs)
{
}

StateMachine::~StateMachine()
{
}

void StateMachine::AddState(const wstring& stateName, shared_ptr<BaseState> state)
{
	if (state == nullptr)
		return;


	if (_curState == nullptr)
	{
		_curStateName = Utils::ToString(stateName);
		_curState = state;
	}

	_states.insert({ stateName, state });
}

void StateMachine::ChangeState(const wstring& stateName)
{
	if (_states[stateName] == nullptr || _curState == _states[stateName])
		return;

	_curState->OnExit();

	_curState = _states[stateName];
	_curStateName = Utils::ToString(stateName);

	_curState->OnEnter();
}