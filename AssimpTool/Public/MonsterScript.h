#pragma once
#include "MonoBehaviour.h"

enum class MonsterState
{
	BORN,
	IDLE,
	CHASE,
	ATTACK_READY,
	ATTACK,
	ONHIT,
	DIE
};

class MonsterScript abstract : public MonoBehaviour
{
public:
	MonsterState GetMonsterState() const { return _curState; }
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return nullptr; }

protected:
	virtual void EnterState(MonsterState state) {}
	virtual void ExitState(MonsterState state) {}

	void ChangeState(MonsterState state)
	{
		if (_curState == state) return;

		ExitState(_curState);

		_curState = state;

		EnterState(_curState);
	}

protected:
	MonsterState _curState = MonsterState::BORN;
	float _maxHp = 1000.f;
	float _hp = _maxHp;
	float _onHitDelay = 0.5f;
	float _curHitDelay = 0.f;
	float _attackZoneRange = 1.5f;
};

