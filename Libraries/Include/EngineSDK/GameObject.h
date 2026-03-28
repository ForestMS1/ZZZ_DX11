#pragma once
#include "Prototype.h"

NS_BEGIN(Engine)

class ENGINE_DLL GameObject : public Prototype
{
protected:
	GameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
	GameObject(const GameObject& Prototype);

public:
	virtual ~GameObject();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Awake() {}
	virtual void	Start() {}
	virtual void	Update() {}
	virtual void	LateUpdate() {}
	virtual void	FixedUpdate() {}
	virtual HRESULT	Render() { return S_OK; }

protected:
	ComPtr<ID3D11Device> _device = { nullptr };
	ComPtr<ID3D11DeviceContext> _deviceContext = { nullptr };


public:
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override;
};

NS_END