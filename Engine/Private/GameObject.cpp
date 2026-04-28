#include "pch.h"
#include "GameObject.h"
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
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	AddComponent(Transform::Create());
	UuidCreate(&_objectId);
	return S_OK;
}

void	GameObject::Awake()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component != nullptr && component->GetLifeState() == LIFESTATE::NONE)
		{
			component->Awake();
			component->SetLifeState(LIFESTATE::AWAKED);
		}
	}

	for (shared_ptr<MonoBehaviour> script : _scripts)
	{
		if (script->GetLifeState() == LIFESTATE::NONE)
		{
			script->Awake();
			script->SetLifeState(LIFESTATE::AWAKED);
		}
	}
}

void	GameObject::Start()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component != nullptr && component->GetLifeState() == LIFESTATE::AWAKED)
		{
			component->Start();
			component->SetLifeState(LIFESTATE::STARTED);
		}
	}

	for (shared_ptr<MonoBehaviour> script : _scripts)
	{
		if (script->GetLifeState() == LIFESTATE::AWAKED)
		{
			script->Start();
			script->SetLifeState(LIFESTATE::STARTED);
		}
	}
}

void	GameObject::Update()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component != nullptr && component->GetLifeState() == LIFESTATE::STARTED)
			component->Update();
	}

	for (shared_ptr<MonoBehaviour> script : _scripts)
	{
		if (script->GetLifeState() == LIFESTATE::STARTED)
			script->Update();
	}
}

void	GameObject::LateUpdate()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component != nullptr && component->GetLifeState() == LIFESTATE::STARTED)
			component->LateUpdate();
	}

	for (shared_ptr<MonoBehaviour> script : _scripts)
	{
		if (script->GetLifeState() == LIFESTATE::STARTED)
			script->LateUpdate();
	}
}

void	GameObject::FixedUpdate()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component != nullptr && component->GetLifeState() == LIFESTATE::STARTED)
			component->FixedUpdate();
	}

	for (shared_ptr<MonoBehaviour> script : _scripts)
	{
		if (script->GetLifeState() == LIFESTATE::STARTED)
			script->FixedUpdate();
	}
}

void	GameObject::EndOfFrame()
{
	// 죽은 컴포넌트들 삭제
	for (int i = 0; i < FIXED_COMPONENT_COUNT; ++i)
	{
		if (_components[i] != nullptr && _components[i]->GetLifeState() == LIFESTATE::REMOVE)
		{
			_components[i]->SetGameObject(nullptr);
			_components[i]->OnDestroy();
			_components[i] = nullptr;
		}
	}
	for (auto iter = _scripts.begin(); iter != _scripts.end();)
	{
		if ((*iter)->GetLifeState() == LIFESTATE::REMOVE)
		{
			(*iter)->SetGameObject(nullptr);
			(*iter)->OnDestroy();
			iter = _scripts.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

HRESULT	GameObject::Render()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component != nullptr && component->GetLifeState() == LIFESTATE::STARTED)
		{
			if(FAILED(component->Render()))
				return E_FAIL;
		}
	}
	//if (_components[static_cast<uint8>(ComponentType::MeshRenderer)])
	//	return _components[static_cast<uint8>(ComponentType::MeshRenderer)]->Render();
	//
	//if (_components[static_cast<uint8>(ComponentType::ModelRenderer)])
	//	return _components[static_cast<uint8>(ComponentType::ModelRenderer)]->Render();
	//
	//if (_components[static_cast<uint8>(ComponentType::Animator)])
	//	return _components[static_cast<uint8>(ComponentType::Animator)]->Render();
	//
	//if (_components[static_cast<uint8>(ComponentType::SpriteRenderer)])
	//	return _components[static_cast<uint8>(ComponentType::SpriteRenderer)]->Render();
	//
	return S_OK;
}

HRESULT GameObject::RenderShadow()
{
	for (shared_ptr<Component> component : _components)
	{
		if (component != nullptr && component->GetLifeState() == LIFESTATE::STARTED)
		{
			if (FAILED(component->RenderShadow()))
				return E_FAIL;
		}
	}
	return S_OK;
}

void GameObject::SetLifeState(LIFESTATE eLifeState)
{ 
	_lifeState = eLifeState;
	if (_lifeState == LIFESTATE::REMOVE)
	{
		for (auto& component : _components)
		{
			if(component)
				component->SetLifeState(LIFESTATE::REMOVE);
		}
		for (auto& script : _scripts)
			script->SetLifeState(LIFESTATE::REMOVE);

		for (auto& childTransform : GetTransform()->GetChildrenTransform())
			childTransform->GetGameObject()->SetLifeState(_lifeState);
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

shared_ptr<Camera> GameObject::GetCamera()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Camera);
	return static_pointer_cast<Camera>(component);
}

shared_ptr<MeshRenderer> GameObject::GetMeshRenderer()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::MeshRenderer);
	return static_pointer_cast<MeshRenderer>(component);
}

shared_ptr<ModelRenderer> GameObject::GetModelRenderer()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::ModelRenderer);
	return static_pointer_cast<ModelRenderer>(component);
}

shared_ptr<ModelAnimator> GameObject::GetModelAnimator()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Animator);
	return static_pointer_cast<ModelAnimator>(component);
}

shared_ptr<SpriteRenderer> GameObject::GetSpriteRenderer()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::SpriteRenderer);
	return static_pointer_cast<SpriteRenderer>(component);
}

shared_ptr<Collider> GameObject::GetCollider()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Collider);
	return static_pointer_cast<Collider>(component);
}

shared_ptr<Light> GameObject::GetLight()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Light);
	return static_pointer_cast<Light>(component);
}

shared_ptr<Transform> GameObject::GetOrAddTransform()
{
	if (nullptr == GetTransform())
	{
		shared_ptr<Transform> transform = Transform::Create();
		AddComponent(transform);
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

void GameObject::RemoveFixedComponent(ComponentType eType)
{
	// 트랜스폼은 지울 수 없음
	if (eType == ComponentType::Transform)
		return;

	uint8 index = static_cast<uint8>(eType);
	if (index < FIXED_COMPONENT_COUNT)
	{
		_components[index]->SetLifeState(LIFESTATE::REMOVE);
	}
}
