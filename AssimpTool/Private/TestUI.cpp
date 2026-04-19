#include "pch.h"
#include "TestUI.h"
#include "Transform.h"
#include "Model.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "SpriteRenderer.h"
TestUI::TestUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

TestUI::TestUI(const TestUI& Prototype)
	: GameObject(Prototype)
{
}

TestUI::~TestUI()
{
}

HRESULT TestUI::Initialize_Prototype()
{

	return S_OK;
}

HRESULT TestUI::Initialize(void* pArg)
{
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	GameObject::Initialize(pArg);
	GameObject::SetName(L"TestUI");
	GameObject::Set_ClassName(L"TestUI");

	shared_ptr<Shader> shader = GAME.GetResource<Shader>(L"UI.fx");

	shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>(shader);
	spriteRenderer->SetSpeed(100.f);
	spriteRenderer->SetLoop(true);
	//for (size_t i = 0; i < 38; ++i)
	//{
	//	shared_ptr<Texture> texture = GAME.GetResource<Texture>(L"LogoBack_" + to_wstring(i));
	//	spriteRenderer->Add_Texture(texture);
	//}
	AddComponent(spriteRenderer);
	spriteRenderer->SetSRV(GAME.GetNormalSRV());

	GetTransform()->SetScale(Vec3(1.f));

	return S_OK;
}

unique_ptr<TestUI> TestUI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<TestUI>(new TestUI(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TestUI");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> TestUI::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new TestUI(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TestUI");
		return nullptr;
	}

	return pInstance;
}