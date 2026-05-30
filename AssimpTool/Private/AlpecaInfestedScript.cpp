#include "pch.h"
#include "AlpecaInfestedScript.h"


#include "AnimFSM.h"
#include "Transition.h"
#include "AnimState.h"
#include "NetworkView.h"
#include "ParticleSystem.h"

#include "TagManagerScript.h"
void AlpecaInfestedScript::Awake()
{

}
void AlpecaInfestedScript::Start()
{

}

void AlpecaInfestedScript::Update()
{
	auto animator = GetGameObject()->GetModelAnimator();
	switch (_curState)
	{
	case MonsterState::BORN:
		BornUpdate();
		break;
	case MonsterState::IDLE:
		IdleUpdate();
		break;
	case MonsterState::CHASE:
		ChaseUpdate();
		break;
	case MonsterState::ATTACK_READY:
		AttackReadyUpdate();
		break;
	case MonsterState::ATTACK:
		AttackUpdate();
		break;
	default:
		break;
	}
}

void AlpecaInfestedScript::OnCollisionEnter(const Collision& collision)
{
	auto collider = GetGameObject()->GetCollider();
	if (collider == nullptr)
		return;

	collider->SetDebugColor(Colors::Red);
}

void AlpecaInfestedScript::OnCollisionStay(const Collision& collision)
{
	_curHitDelay += DT;
	if (collision.colliderA.lock()->GetGameObject()->GetName().find(L"Weapon") != wstring::npos || 
		collision.colliderB.lock()->GetGameObject()->GetName().find(L"Weapon") != wstring::npos)
	{
		if (auto animator = GetGameObject()->GetModelAnimator())
		{
			if (_curHitDelay >= _onHitDelay)
			{
				animator->SetTrigger(L"onHit");
				_curHitDelay = 0.f;
				// ==================== [ 파티클 이펙트 생성 구간 ] ====================
				shared_ptr<GameObject> particleObj = make_shared<GameObject>(DEVICE, CONTEXT);
				particleObj->Initialize();
				particleObj->SetName(L"particle");

				// 부모 오브젝트의 스케일은 기본값(1)으로 두어 자식 파티클 행렬이 뻥튀기되는 것을 방지합니다.
				particleObj->GetTransform()->SetScale(Vec3(1.f, 1.f, 1.f));
				particleObj->GetTransform()->SetPosition(_gameObject.lock()->GetTransform()->GetLocalPosition());
				particleObj->SetLifeState(LIFESTATE::NONE);

				shared_ptr<ParticleSystem> ps = make_shared<ParticleSystem>();
				ps->SetTexture(GAME.GetOrAddTexture(L"Eff_Assault_LC_06", L"../../Resources/Textures/UI/Eff_Assault_LC_06.png"));
				particleObj->AddComponent(ps);

				// 파티클 설정 값 최적화
				Vec3 startPos = _gameObject.lock()->GetTransform()->GetLocalPosition();
				Vec3 minVel = { -5.f,  2.f, -5.f }; // 좌, 상, 후 방향으로 퍼짐
				Vec3 maxVel = { 5.f,  8.f,  5.f }; // 우, 상, 전 방향으로 팍 터짐
				Vec3 pScale = { 0.3f, 0.3f, 0.3f }; // 알갱이 하나당 현실적인 크기 (30cm)
				float lifeTime = 1.5f;                // 이펙트는 보통 1초~2초 내외가 가장 예쁩니다.
				uint32 count = 100;

				ps->CreateParticles(startPos, minVel, maxVel, pScale, lifeTime, count);

				GAME.Add_GameObject_toLayerNoClone(ETOUI(LEVEL::TESTMESH), L"Layer_UI", particleObj);
			}
		}
	}
}

void AlpecaInfestedScript::OnCollisionExit(const Collision& collision)
{
	auto collider = GetGameObject()->GetCollider();
	if (collider == nullptr)
		return;

	collider->SetDebugColor(Colors::LimeGreen);

	_curHitDelay = 0.f;
}

void AlpecaInfestedScript::OnDestroy()
{
}


unique_ptr<AlpecaInfestedScript> AlpecaInfestedScript::Create()
{
	auto pInstance = unique_ptr<AlpecaInfestedScript>(new AlpecaInfestedScript);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : AlpecaInfestedScript");
		return nullptr;
	}

	return pInstance;
}

void AlpecaInfestedScript::ExitState(MonsterState state)
{
	auto animator = GetGameObject()->GetModelAnimator();
	switch (state)
	{
	case MonsterState::BORN:
		break;
	case MonsterState::IDLE:
		break;
	case MonsterState::CHASE:
		animator->SetBool(L"isMove", false);
		break;
	case MonsterState::ATTACK_READY:
		break;
	case MonsterState::ATTACK:
		break;
	case MonsterState::ONHIT:
		break;
	case MonsterState::DIE:
		break;
	default:
		break;
	}
}

void AlpecaInfestedScript::EnterState(MonsterState state)
{
	auto animator = GetGameObject()->GetModelAnimator();
	switch (state)
	{
	case MonsterState::BORN:
		animator->SetTrigger(L"born");
		break;
	case MonsterState::IDLE:
		animator->SetTrigger(L"idle");
		break;
	case MonsterState::CHASE:
		animator->SetTrigger(L"walk");
		break;
	case MonsterState::ATTACK_READY:
		animator->SetTrigger(L"attack01");
		break;
	case MonsterState::ATTACK:
		break;
	case MonsterState::ONHIT:
		animator->SetTrigger(L"onHit");
		break;
	case MonsterState::DIE:
		break;
	default:
		break;
	}
}

void AlpecaInfestedScript::BornUpdate()
{
	auto animator = GetGameObject()->GetModelAnimator();

	// Born 애니메이션 끝났으면
	if(animator->IsCurrentAnimFinished())
		ChangeState(MonsterState::IDLE);
}

void AlpecaInfestedScript::IdleUpdate()
{
	_targetPlayer = GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"TagManager")->GetScript<TagManagerScript>()->GetCurCharacter();
	if (_targetPlayer == nullptr)
		return;

	// 추격감지 범위 내에 들어왔다면
	auto targetTransform = _targetPlayer->GetTransform();
	float length = (GetTransform()->GetPosition() - targetTransform->GetPosition()).Length();

	if (_chaseZoneRange >= length);
		ChangeState(MonsterState::CHASE);
}

void AlpecaInfestedScript::ChaseUpdate()
{
	_targetPlayer = GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"TagManager")->GetScript<TagManagerScript>()->GetCurCharacter();
	if (_targetPlayer == nullptr)
		return;

	auto animator = GetGameObject()->GetModelAnimator();
	animator->SetBool(L"isMove", true);

	auto targetTransform = _targetPlayer->GetTransform();
	GetTransform()->Chase(targetTransform->GetPosition(), 0.5f, 2.f);

	//공격 범위 내에 들어왔다면
	float length = (GetTransform()->GetPosition() - targetTransform->GetPosition()).Length();
	if(_attackZoneRange >= length)
		ChangeState(MonsterState::ATTACK_READY);
}

void AlpecaInfestedScript::AttackReadyUpdate()
{
	_targetPlayer = GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"TagManager")->GetScript<TagManagerScript>()->GetCurCharacter();
	if (_targetPlayer == nullptr)
		return;

	//공격 범위 벗어났다면
	auto targetTransform = _targetPlayer->GetTransform();
	float length = (GetTransform()->GetPosition() - targetTransform->GetPosition()).Length();
	if (_attackZoneRange <= length)
		ChangeState(MonsterState::IDLE);

	// TODO : 눈 번쩍
	auto animator = GetGameObject()->GetModelAnimator();

	// 애니메이션 재생비율 0.2이상
	if (animator->GetProgress() >= 0.2f)
		ChangeState(MonsterState::ATTACK);
}

void AlpecaInfestedScript::AttackUpdate()
{
	_targetPlayer = GAME.Find_GameObject_fromLayer(L"Layer_Basic", L"TagManager")->GetScript<TagManagerScript>()->GetCurCharacter();
	if (_targetPlayer == nullptr)
		return;

	auto animator = GetGameObject()->GetModelAnimator();
	// Attack 애니메이션 끝났으면
	if (animator->IsCurrentAnimFinished())
		ChangeState(MonsterState::IDLE);
}


void AlpecaInfestedScript::OnInspectorGUI()
{
	GuiRemoveButton("AlpecaInfestedScript");
	if (ImGui::CollapsingHeader("AlpecaInfestedScript", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
}