#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class Transform;
class MonoBehaviour;

class ENGINE_DLL GameObject abstract : public Prototype
{
protected:
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	GameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	GameObject(const GameObject& Prototype);

public:
	virtual ~GameObject();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);

	void	Awake();
	void	Start();
	void	Update();
	void	LateUpdate();
	void	FixedUpdate();

	virtual HRESULT	Render();

	// LIFESTATE
	LIFESTATE GetLifeState() const { return _lifeState; }
	void	  SetLifeState(LIFESTATE eLifeState) { _lifeState = eLifeState; }

	// Component
	shared_ptr<Component> GetFixedComponent(ComponentType eType);
	shared_ptr<Transform> GetTransform();

	shared_ptr<Transform> GetOrAddTransform();
	void AddComponent(shared_ptr<Component> component);

protected:
	ComPtr<ID3D11Device> _device = { nullptr };
	ComPtr<ID3D11DeviceContext> _deviceContext = { nullptr };
	LIFESTATE			_lifeState = { LIFESTATE::NONE };

protected:
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
	vector<shared_ptr<MonoBehaviour>> _scripts;

public:
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) = 0;
};

NS_END