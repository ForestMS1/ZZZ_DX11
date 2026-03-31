#include "pch.h"
#include "GameObject.h"
#include "MonoBehaviour.h"
#include "Transform.h"
#include "GameInstance.h"

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


shared_ptr<Component> GameObject::GetFixedComponent(ComponentType eType)
{
	uint8 index = static_cast<uint8>(eType);

	assert(index < FIXED_COMPONENT_COUNT);

	return _components[index];
}

shared_ptr<Transform> GameObject::GetTransform()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Transform);
	return static_pointer_cast<Transform>(component);
}

shared_ptr<Transform> GameObject::GetOrAddTransform()
{
	if (nullptr == GetTransform())
	{
		// TODO : make_shared 대신에 프로토타입에서 복사해오기
		//shared_ptr<Transform> transform = make_shared<Transform>();
		//shared_ptr<Transform> transform = dynamic_pointer_cast<Transform>
		//	(GameInstance::Get().Clone_Prototype(0, L"Prototype_Component_Transform", nullptr));
		//AddComponent(transform);
	}

	return GetTransform();
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	component->SetGameObject(SHARED_THIS(GameObject));

	uint8 index = static_cast<uint8>(component->GetType());
	if (index < FIXED_COMPONENT_COUNT)
	{
		_components[index] = component;
	}
	else
	{
		_scripts.push_back(dynamic_pointer_cast<MonoBehaviour>(component));
	}
}
