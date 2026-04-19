#include "pch.h"
#include "TestSphere.h"
#include "Transform.h"
#include "Model.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "TestSphereScript.h"
#include "SpriteRenderer.h"
#include "OBBCollider.h"
TestSphere::TestSphere(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

TestSphere::TestSphere(const TestSphere& Prototype)
	: GameObject(Prototype)
{
}

TestSphere::~TestSphere()
{
}

HRESULT TestSphere::Initialize_Prototype()
{

	return S_OK;
}

HRESULT TestSphere::Initialize(void* pArg)
{
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	GameObject::Initialize(pArg);
	GameObject::SetName(L"TestSphere");
	GameObject::Set_ClassName(L"TestSphere");
	AddComponent(make_shared<TestSphereScript>());

	//shared_ptr<Shader> shader = GAME.GetResource<Shader>(L"Test.fx");
	//shared_ptr<Model> model = GAME.GetResource<Model>(L"Stage");
	//
	//AddComponent(make_shared<ModelRenderer>(shader));
	//GetModelRenderer()->SetModel(model);

	shared_ptr<Shader> shader = GAME.GetResource<Shader>(L"UI.fx");
	
	shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>(shader);
	spriteRenderer->SetSpeed(100.f);
	spriteRenderer->SetLoop(true);
	for (size_t i = 0; i < 38; ++i)
	{
		shared_ptr<Texture> texture = GAME.GetResource<Texture>(L"LogoBack_" + to_wstring(i));
		spriteRenderer->Add_Texture(texture);
	}
	AddComponent(spriteRenderer);
	spriteRenderer->SetSRV(GAME.GetDepthSRV());

	//shared_ptr<OBBCollider> collider = make_shared<OBBCollider>();
	//AddComponent(collider);


	GetTransform()->SetScale(Vec3(1.f));

	return S_OK;
}

unique_ptr<TestSphere> TestSphere::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<TestSphere>(new TestSphere(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TestSphere");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> TestSphere::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new TestSphere(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TestSphere");
		return nullptr;
	}

	return pInstance;
}