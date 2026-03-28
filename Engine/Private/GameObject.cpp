#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: _device(pDevice)
	, _deviceContext(pDeviceContext)
{
}

GameObject::GameObject(const GameObject& Prototype)
	: _device(Prototype._device)
	, _deviceContext(Prototype._deviceContext)
{
}

GameObject::~GameObject()
{
}

HRESULT GameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT GameObject::Initialize(void* pArg)
{
	return S_OK;
}

shared_ptr<Prototype> GameObject::Clone(void* pArg)
{
	return shared_ptr<Prototype>();
}
