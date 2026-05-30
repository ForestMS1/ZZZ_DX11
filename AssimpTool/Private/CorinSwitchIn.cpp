#include "pch.h"
#include "CorinSwitchIn.h"
#include "CorinStateMachineScript.h"
#include "AnimState.h"
#include "ParticleSystem.h"
CorinSwitchIn::CorinSwitchIn(shared_ptr<GameObject> gameObject, shared_ptr<StateMachine> stateMachine)
	: BaseState(gameObject, stateMachine)
{
}

CorinSwitchIn::~CorinSwitchIn()
{
}

void CorinSwitchIn::OnEnter()
{
	auto animator = _animator.lock();
	animator->SetTrigger(L"switchInAttack");

	animator->SetCurRenderGroup(RENDERGROUP::NONBLEND);
	animator->SetPass(0);

	auto shader = _animator.lock()->GetShader();
	shader->GetScalar("g_AlphaValue")->SetFloat(1.f);

	auto collider = _gameObject.lock()->GetCollider();
	collider->SetActive(true);


	// ==================== [ 파티클 이펙트 생성 구간 ] ====================
	shared_ptr<GameObject> particleObj = make_shared<GameObject>(DEVICE, CONTEXT);
	particleObj->Initialize();
	particleObj->SetName(L"particle");

	// 부모 오브젝트의 스케일은 기본값(1)으로 두어 자식 파티클 행렬이 뻥튀기되는 것을 방지합니다.
	particleObj->GetTransform()->SetScale(Vec3(1.f, 1.f, 1.f));
	particleObj->GetTransform()->SetPosition(_gameObject.lock()->GetTransform()->GetLocalPosition());
	particleObj->SetLifeState(LIFESTATE::NONE);

	shared_ptr<ParticleSystem> ps = make_shared<ParticleSystem>();
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
