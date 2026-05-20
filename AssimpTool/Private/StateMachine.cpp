#include "pch.h"
#include "StateMachine.h"
#include "BaseState.h"
#include "AnimState.h"
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

void StateMachine::Awake()
{
	_animator = GetGameObject()->GetModelAnimator();
}

void StateMachine::AddState(const wstring& stateName, shared_ptr<BaseState> state)
{
	if (state == nullptr)
		return;


	if (_curState == nullptr)
	{
		_curStateName = Utils::ToString(stateName);
		_curState = state;
		_curState->OnEnter();
	}

	_states.insert({ stateName, state });
}


void StateMachine::ChangeState(const wstring& stateName)
{
	auto iter = _states.find(stateName);
	if (iter == _states.end())
		return;

	if(_curState != nullptr)
		_curState->OnExit();

	_curState = iter->second;
	_curStateName = Utils::ToString(stateName);

	_curState->OnEnter();
}

wstring StateMachine::GetCurAnimStateName()
{
	return _animator.lock()->GetFSM()->GetCurAnimState()->GetName();
}

void StateMachine::ChangeCurAnimState(const wstring& animStateName)
{
	_animator.lock()->GetFSM()->ChangeState(animStateName);
}
