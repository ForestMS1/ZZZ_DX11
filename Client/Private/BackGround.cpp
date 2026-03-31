#include "pch.h"
#include "BackGround.h"
#include "BackGroundTestScript.h"

BackGround::BackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

BackGround::BackGround(const BackGround& Prototype)
	: GameObject(Prototype)
{
}

BackGround::~BackGround()
{
}

HRESULT BackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT BackGround::Initialize(void* pArg)
{
	AddComponent(make_shared<BackGroundTestScript>());
	return S_OK;
}

HRESULT BackGround::Render()
{
	return S_OK;
}

unique_ptr<BackGround> BackGround::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<BackGround>(new BackGround(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : BackGround");
		return nullptr;
	}
	
	return pInstance;
}

shared_ptr<Prototype> BackGround::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new BackGround(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BackGround");
		return nullptr;
	}

	return pInstance;
}