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
	virtual void Update() override;

	MonsterState GetMonsterState() const { return _curState; }
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return nullptr; }

public:
	virtual void OnHit(uint32 damage);

protected:
	virtual void CreateOnHitParticle(uint32 damage);

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
	uint32 _maxHp = 10000;
	uint32 _hp = _maxHp;
	float _hitDelayLimit = 0.25f;
	float _hitDelayTimer = 0.f;
	float _attackZoneRange = 1.5f;
	float _chaseZoneRange = 10.f;
};

