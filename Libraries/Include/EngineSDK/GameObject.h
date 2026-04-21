#pragma once
#include "Transform.h"
#include "Camera.h"
#include "MonoBehaviour.h"
#include "MeshRenderer.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "SpriteRenderer.h"
#include "AABBCollider.h"
#include "OBBCollider.h"
#include "SphereCollider.h"

NS_BEGIN(Engine)

class Layer;
//class Transform;
//class MonoBehaviour;
//
//
//class Camera;
//class MeshRenderer;
//class ModelRenderer;
//class ModelAnimator;

class ENGINE_DLL GameObject : public Prototype
{
public:
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	GameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	GameObject(const GameObject& Prototype);

public:
	virtual ~GameObject();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);

	// 생명주기 함수
	void	Awake();
	void	Start();
	void	Update();
	void	LateUpdate();
	void	FixedUpdate();
	void	EndOfFrame();

	virtual HRESULT	Render();

	// LIFESTATE Property
	LIFESTATE GetLifeState() const { return _lifeState; }
	void	  SetLifeState(LIFESTATE eLifeState);

	// Fixed Component Getter
	shared_ptr<Component>		GetFixedComponent(ComponentType eType);
	shared_ptr<Transform>		GetTransform();
	shared_ptr<Camera>			GetCamera();
	shared_ptr<MeshRenderer>	GetMeshRenderer();
	shared_ptr<ModelRenderer>	GetModelRenderer();
	shared_ptr<ModelAnimator>	GetModelAnimator();
	shared_ptr<SpriteRenderer>	GetSpriteRenderer();
	shared_ptr<Collider>		GetCollider();

	// Add Component
	shared_ptr<Transform> GetOrAddTransform();
	void AddComponent(shared_ptr<Component> component);

	// 선형탐색. Update같은 루프에서 사용하지말고 Awake나 Start에서 캐싱해서 쓰도록
	template<typename T>
	shared_ptr<T>	GetScript()
	{
		for (auto& script : _scripts)
		{
			shared_ptr<T> casted = dynamic_pointer_cast<T>(script);
			if (casted)
				return casted;
		}
		return nullptr;
	}

	// Remove Component
	void RemoveFixedComponent(ComponentType eType);

	template<typename T>
	void RemoveScriptComponent()
	{
		for (auto& script : _scripts)
		{
			if (dynamic_pointer_cast<T>(script))
			{
				script->SetLifeState(LIFESTATE::REMOVE);
				break; 
			}
		}
	}

	// name
	void SetName(const wstring& name) { _name = name; }
	const wstring& GetName() const { return _name; }
	void Set_ClassName(const wstring& className) { _className = className; }
	const wstring& Get_ClassName() const { return _className; } //GetClassName은 define 겹치는게있네

	// Id
	UUID GetId() { return _objectId; }
	void SetId(UUID id) { _objectId = id; }
	// 클래스


	// 소속 Layer
	Layer* GetMyLayer() { return _layer; }
	void SetMyLayer(Layer* layer) { _layer = layer; }

	// ImGui Inspector 정보를 위해 열어둠
	const array<shared_ptr<Component>, FIXED_COMPONENT_COUNT>& GetComponents() { return _components; }
	const vector<shared_ptr<MonoBehaviour>>& GetScripts() { return _scripts; }

protected:
	ComPtr<ID3D11Device> _device = { nullptr };
	ComPtr<ID3D11DeviceContext> _deviceContext = { nullptr };
	LIFESTATE			_lifeState = { LIFESTATE::NONE };

protected:
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
	vector<shared_ptr<MonoBehaviour>> _scripts;

	wstring _name = L"Empty";
	// 클래스 이름 (ex : Corin.h/Corin.cpp 면 Corin으로 저장)
	wstring _className = L"GameObject";
	Layer* _layer;

	UUID _objectId = {};

public:
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return nullptr; }
};

NS_END