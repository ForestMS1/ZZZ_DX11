#pragma once
#include "MonsterScript.h"

class AlpecaInfestedScript : public MonsterScript
{
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnCollisionEnter(const Collision& collision) override;
	virtual void OnCollisionStay(const Collision& collision) override;
	virtual void OnCollisionExit(const Collision& collision) override;

	virtual void OnDestroy() override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return make_shared<AlpecaInfestedScript>(*this); }

	virtual void OnInspectorGUI() override;

public:
	static unique_ptr<AlpecaInfestedScript> Create();

private:
	shared_ptr<GameObject> _targetPlayer;

	virtual void ExitState(MonsterState state) override;
	virtual void EnterState(MonsterState state) override;

	void BornUpdate();
	void IdleUpdate();
	void ChaseUpdate();
	void AttackReadyUpdate();
	void AttackUpdate();
};

