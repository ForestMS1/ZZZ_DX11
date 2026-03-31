#pragma once

NS_BEGIN(Engine)

class GameObject;

class ENGINE_DLL Renderer final
{
private:
	Renderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
public:
	~Renderer();

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<GameObject> pRenderObject);
	HRESULT Draw();

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();


private:
	ComPtr<ID3D11Device> _device = { nullptr };
	ComPtr<ID3D11DeviceContext> _deviceContext = { nullptr };

private:
	list<shared_ptr<GameObject>> _renderObjects[ETOUI(RENDERGROUP::END)];


public:
	static unique_ptr<Renderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
};

NS_END
