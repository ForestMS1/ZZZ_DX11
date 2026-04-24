#include "pch.h"
#include "LightManager.h"
#include "Light.h"
LightManager::LightManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: _device(pDevice)
	, _deviceContext(pDeviceContext)
{
}

LightManager::~LightManager()
{
}

HRESULT LightManager::Initialize()
{

	return S_OK;
}

HRESULT LightManager::Add_LightObject(shared_ptr<Light> pLight)
{
	if (pLight == nullptr)
		return E_FAIL;

	if (pLight->GetLifeState() == LIFESTATE::REMOVE)
		return E_FAIL;

	_lights.push_back(pLight);

	return S_OK;
}

HRESULT LightManager::Clear_LightObject()
{
	_lights.clear();
	return S_OK;
}

unique_ptr<LightManager> LightManager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<LightManager>(new LightManager(pDevice, pDeviceContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Cloned : LightManager");
		return nullptr;
	}

	return pInstance;
}

