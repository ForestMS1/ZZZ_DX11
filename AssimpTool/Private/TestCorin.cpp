#include "pch.h"
#include "TestCorin.h"
#include "TestCorinScript.h"

#include "AnimState.h"
#include "Transition.h"
#include "Condition.h"
#include "AABBCollider.h"
TestCorin::TestCorin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

TestCorin::TestCorin(const TestCorin& Prototype)
	: GameObject(Prototype)
{
}

TestCorin::~TestCorin()
{
}

HRESULT TestCorin::Initialize_Prototype()
{

	return S_OK;
}

HRESULT TestCorin::Initialize(void* pArg)
{
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	GameObject::Initialize(pArg);
	GameObject::SetName(L"TestCorin");
	GameObject::Set_ClassName(L"TestCorin");
	AddComponent(make_shared<TestCorinScript>());

	shared_ptr<Shader> shader = GAME.GetResource<Shader>(L"TweenTest.fx");
	shared_ptr<Model> model = GAME.GetResource<Model>(L"CorinAnimModel");

	shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
	animator->SetModel(model);
	shared_ptr<AnimFSM> fsm = make_shared<AnimFSM>(animator);
	fsm->Load("CorinFSM", animator);
	//animator->SetFSM(fsm);
	//shared_ptr<AnimState> stateIdle = make_shared<AnimState>(fsm);
	//shared_ptr<AnimState> stateWalk = make_shared<AnimState>(fsm);
	//stateIdle->SetAnimationClip(model->GetAnimationByName(L"Avatar_Female_Size01_Corin_Ani_Idle"));
	//stateWalk->SetAnimationClip(model->GetAnimationByName(L"Avatar_Female_Size01_Corin_Ani_Walk"));
	//shared_ptr<Transition> transition_Idle_ToWalk = make_shared<Transition>(fsm);
	//shared_ptr<Transition> transition_Walk_ToIdle = make_shared<Transition>(fsm);
	//transition_Idle_ToWalk->SetToState(stateWalk);
	//transition_Walk_ToIdle->SetToState(stateIdle);
	//
	//fsm->AddBool(L"Move", 0.5f);
	//shared_ptr<FloatCondition> condition1 = make_shared<FloatCondition>();
	//shared_ptr<FloatCondition> condition2 = make_shared<FloatCondition>();
	//condition1->_paramName = L"Move";
	//condition1->_value = 0.2f;
	//condition1->_mode = ConditionMode::Greater;
	//
	//condition2-> _paramName = L"Move";
	//condition2->_value = 0.2f;
	//condition2->_mode = ConditionMode::Less;
	//
	//transition_Idle_ToWalk->Add_Condition(condition1);
	//transition_Walk_ToIdle->Add_Condition(condition2);
	//
	//stateIdle->Add_Transition(transition_Idle_ToWalk);
	//stateWalk->Add_Transition(transition_Walk_ToIdle);
	//fsm->Add_AnimState(L"Idle", stateIdle);
	//fsm->Add_AnimState(L"Walk", stateWalk);
	//fsm->ChangeState(L"Idle");
	//
	animator->SetFSM(fsm);
	AddComponent(animator);

	shared_ptr<AABBCollider> collider = make_shared<AABBCollider>();
	AddComponent(collider);

	GetTransform()->SetPosition(Vec3(0, 0, 0));
	GetTransform()->SetScale(Vec3(1.f));

	return S_OK;
}

unique_ptr<TestCorin> TestCorin::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<TestCorin>(new TestCorin(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TestCorin");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> TestCorin::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new TestCorin(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TestCorin");
		return nullptr;
	}

	return pInstance;
}