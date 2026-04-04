#include "pch.h"
#include "TestCorin.h"
#include "TestCorinScript.h"
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
	AddComponent(make_shared<TestCorinScript>());

	shared_ptr<Shader> shader = make_shared<Shader>(L"TweenTest.fx");

	shared_ptr<Model> model = make_shared<Model>();
	model->ReadModel(L"Corin/Corin");
	model->ReadMaterial(L"Corin/Corin");
	model->ReadAnimation(L"Corin/Avatar_Female_Size01_Corin_Ani_Walk_Start");
	model->ReadAnimation(L"Corin/Avatar_Female_Size01_Corin_Ani_Walk");
	model->ReadAnimation(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchOut_Normal");

	AddComponent(make_shared<ModelAnimator>(shader));
	GetModelAnimator()->SetModel(model);

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