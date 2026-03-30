#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL Level abstract
{
protected:
	Level(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
public:
	virtual ~Level() = default;

public:
	virtual HRESULT Initialize();
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Device> _device = { nullptr };
	ComPtr<ID3D11DeviceContext> _deviceContext = { nullptr };


};

NS_END
