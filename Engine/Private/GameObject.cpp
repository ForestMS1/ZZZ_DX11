#include "pch.h"
#include "GameObject.h"
#include "MonoBehaviour.h"

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

void	GameObject::Awake()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component)
			component->Awake();
	}

	for (shared_ptr<MonoBehaviour> script : _scripts)
	{
		script->Awake();
	}
}

void	GameObject::Start()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component)
			component->Start();
	}

	for (shared_ptr<MonoBehaviour> script : _scripts)
	{
		script->Start();
	}
}

void	GameObject::Update()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component)
			component->Update();
	}

	for (shared_ptr<MonoBehaviour> script : _scripts)
	{
		script->Update();
	}
}

void	GameObject::LateUpdate()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component)
			component->LateUpdate();
	}

	for (shared_ptr<MonoBehaviour> script : _scripts)
	{
		script->LateUpdate();
	}
}

void	GameObject::FixedUpdate()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component)
			component->FixedUpdate();
	}

	for (shared_ptr<MonoBehaviour> script : _scripts)
	{
		script->FixedUpdate();
	}
}
