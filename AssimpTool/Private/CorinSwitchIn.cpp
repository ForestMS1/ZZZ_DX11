#include "pch.h"
#include "CorinSwitchIn.h"
#include "CorinStateMachineScript.h"
#include "AnimState.h"
#include "ParticleSystem.h"
#include "MonsterScript.h"

CorinSwitchIn::CorinSwitchIn(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinSwitchIn::~CorinSwitchIn()
{
}

void CorinSwitchIn::OnEnter()
{
	// -------------------------------------------근처에 공격중인 몬스터 있는지 검사--------------------------------------------
	auto transform = _gameObject.lock()->GetTransform();
	const auto& targets = _stateMachine.lock()->GetNearMonsters();
	if (!targets.empty())
	{
		for (const auto& monster : targets)
		{
			if (auto script = monster->GetScript<MonsterScript>())
			{
				if (script->GetMonsterState() == MonsterState::ATTACK_READY || script->GetMonsterState() == MonsterState::ATTACK)
				{
					Vec3 targetPos = monster->GetTransform()->GetPosition();

					Vec3 monsterLook = monster->GetTransform()->GetLocalLook();
					monsterLook.Normalize();
					Vec3 spawnPos = targetPos + monsterLook * 1.5f;
					transform->SetLocalPosition(spawnPos);
					transform->LookAtLocalY(targetPos);
					_stateMachine.lock()->ChangeState(L"CorinParry");
					return;
				}
			}
		}
	}
	// ------------------------------------------------------------------------------------------------------------------------

	auto animator = _animator.lock();
	animator->SetTrigger(L"switchInAttack");

	animator->SetCurRenderGroup(RENDERGROUP::NONBLEND);
	animator->SetPass(0);

	auto shader = _animator.lock()->GetShader();
	shader->GetScalar("g_AlphaValue")->SetFloat(1.f);

	auto collider = _gameObject.lock()->GetCollider();
	collider->SetActive(true);
}

void CorinSwitchIn::Input()
{

}

void CorinSwitchIn::Awake()
{
}

void CorinSwitchIn::Start()
{
}

void CorinSwitchIn::Update()
{
	const auto& stateMachine = _stateMachine.lock();
	if (stateMachine->GetCurAnimStateName() == L"Idle")
		_stateMachine.lock()->ChangeState(L"CorinIdle");
}

void CorinSwitchIn::LateUpdate()
{

}

void CorinSwitchIn::FixedUpdate()
{
}

HRESULT CorinSwitchIn::Render()
{
	return S_OK;
}

void CorinSwitchIn::OnCollisionEnter(const Collision& collision)
{
}

void CorinSwitchIn::OnCollisionExit(const Collision& collision)
{
}

void CorinSwitchIn::OnExit()
{
}
