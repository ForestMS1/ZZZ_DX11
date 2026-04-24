#pragma once

NS_BEGIN(Engine)

class Light;

class ENGINE_DLL LightManager
{
private:
	LightManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
public:
	~LightManager();

public:
	HRESULT Initialize();
	HRESULT Add_LightObject(shared_ptr<Light> pLight);
	// 매 프레임 클리어
	HRESULT Clear_LightObject();

	list<shared_ptr<Light>>& GetLigthList() { return _lights; }



private:
	ComPtr<ID3D11Device> _device = { nullptr };
	ComPtr<ID3D11DeviceContext> _deviceContext = { nullptr };

private:
	list<shared_ptr<Light>> _lights;

public:
	static unique_ptr<LightManager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
};

NS_END