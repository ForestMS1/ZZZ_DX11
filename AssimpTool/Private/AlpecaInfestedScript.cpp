#include "pch.h"
#include "AlpecaInfestedScript.h"


#include "AnimFSM.h"
#include "Transition.h"
#include "AnimState.h"
#include "NetworkView.h"
#include "DamageParticleSystem.h"

#include "TagManagerScript.h"

void AlpecaInfestedScript::Awake()
{
	GAME.Subscribe(static_cast<uint32>(EventType::QUEST_START_END), [this](const EventDesc& desc) { this->ChangeState(MonsterState::BORN); });
	//ChangeState(MonsterState::BORN);
}
void AlpecaInfestedScript::Start()
{

}

void AlpecaInfestedScript::Update()
{
	MonsterScript::Update();

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

}

void AlpecaInfestedScript::OnCollisionExit(const Collision& collision)
{
	auto collider = GetGameObject()->GetCollider();
	if (collider == nullptr)
		return;

	collider->SetDebugColor(Colors::LimeGreen);
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
		animator->SetCurRenderGroup(RENDERGROUP::NONBLEND);
		animator->SetPass(0);
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
		_alphaValue = 0.f;
		animator->SetCurRenderGroup(RENDERGROUP::BLEND);
		animator->SetPass(2);
		break;
	case MonsterState::IDLE:
		animator->SetTrigger(L"idle");
		break;
	case MonsterState::CHASE:
		animator->SetTrigger(L"walk");
		break;
	case MonsterState::ATTACK_READY:
		animator->SetTrigger(L"attack01");
		CreateAttackReadyParticle();
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

	_alphaValue += _fadeSpeed * DT;
	if (_alphaValue > 1.f)
		_alphaValue = 1.f;

	auto shader = animator->GetShader();
	shader->GetScalar("g_AlphaValue")->SetFloat(_alphaValue);

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