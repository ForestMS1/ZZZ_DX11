#pragma once
#include "MonoBehaviour.h"

class BaseState;

class StateMachine : public MonoBehaviour
{
public:
	StateMachine();
	StateMachine(const StateMachine& rhs);
	virtual ~StateMachine();

public:
	virtual void Awake() override;
	void ChangeState(const wstring& stateName);

protected:
	void AddState(const wstring& stateName, shared_ptr<BaseState> state);

protected:
	friend class BaseState;
	weak_ptr<ModelAnimator> _animator = {};


protected:
	// 게임로직 상태머신
	// 현재 진행중인 State
	shared_ptr<BaseState> _curState;
	// 상태들 보관
	map<wstring, shared_ptr<BaseState>> _states;

	// ImGui용 현재상태 이름
	string _curStateName;
};