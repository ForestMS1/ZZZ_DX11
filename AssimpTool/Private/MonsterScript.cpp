#include "pch.h"
#include "MonsterScript.h"
#include "DamageParticleSystem.h"

void MonsterScript::Update()
{
	if (_hitDelayTimer > 0.f)
		_hitDelayTimer -= DT;

}

void MonsterScript::OnHit(uint32 damage)
{
	if (_hitDelayTimer > 0.f)
		return;

	_hitDelayTimer = _hitDelayLimit;

	static std::random_device _rd;
	static std::mt19937 _gen{ _rd() };

	float minDmg = (damage > 1000) ? static_cast<float>(damage - 1000) : 1.f;
	float maxDmg = static_cast<float>(damage + 1000);

	std::uniform_real_distribution<float> randomDamage(minDmg, maxDmg);

	// 이번에 최종적으로 들어갈 랜덤 데미지 계산
	uint32 finalDamage = static_cast<uint32>(randomDamage(_gen));

	if (_hp <= finalDamage)
	{
		_hp = 0;
		ChangeState(MonsterState::DIE);
	}
	else
		_hp -= finalDamage;

	if(auto animator = GetGameObject()->GetModelAnimator())
		animator->SetTrigger(L"onHit");
	CreateOnHitParticle(finalDamage);
}

void MonsterScript::CreateOnHitParticle(uint32 damage)
{
	auto transform = _gameObject.lock()->GetTransform();
	// ==================== [ 파티클 이펙트 생성 구간 ] ====================
	shared_ptr<GameObject> particleObj = make_shared<GameObject>(DEVICE, CONTEXT);
	particleObj->Initialize();
	particleObj->SetName(L"particle");

	// 부모 오브젝트의 스케일은 기본값(1)으로 두어 자식 파티클 행렬이 뻥튀기되는 것을 방지
	particleObj->GetTransform()->SetScale(Vec3(1.f, 1.f, 1.f));
	particleObj->GetTransform()->SetPosition(transform->GetLocalPosition());
	particleObj->SetLifeState(LIFESTATE::NONE);

	shared_ptr<DamageParticleSystem> ps = make_shared<DamageParticleSystem>();
	particleObj->AddComponent(ps);


	Vec3 monsterScale = transform->GetLocalScale();
	Vec3 startPos = transform->GetLocalPosition() + Vec3(0.f, monsterScale.y, 0.f);

	ps->SpawnDamageText(startPos, monsterScale, damage);
	GAME.Add_GameObject_toLayerNoClone(ETOUI(LEVEL::TESTMESH), L"Layer_UI", particleObj);
}