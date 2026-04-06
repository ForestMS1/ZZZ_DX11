#include "pch.h"
#include "SkyBox.h"
#include "SkyBoxScript.h"

SkyBox::SkyBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

SkyBox::SkyBox(const SkyBox& Prototype)
	: GameObject(Prototype)
{
}

SkyBox::~SkyBox()
{
}

HRESULT SkyBox::Initialize_Prototype()
{

	return S_OK;
}

HRESULT SkyBox::Initialize(void* pArg)
{
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	GameObject::Initialize(pArg);
	GameObject::SetName(L"SkyBox");
	AddComponent(make_shared<SkyBoxScript>());

	return S_OK;
}

unique_ptr<SkyBox> SkyBox::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<SkyBox>(new SkyBox(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : SkyBox");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> SkyBox::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new SkyBox(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : SkyBox");
		return nullptr;
	}

	return pInstance;
}