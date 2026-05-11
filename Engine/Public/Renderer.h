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
	HRESULT Render_Shadow();
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();
	HRESULT Redner_Collider();

	HRESULT Render_Deferred_Lighting();


private:
	ComPtr<ID3D11Device> _device = { nullptr };
	ComPtr<ID3D11DeviceContext> _deviceContext = { nullptr };

	shared_ptr<class Shader> _finalBindShader = { nullptr };
	shared_ptr<class Shader> _renderTargetShader = { nullptr };
	shared_ptr<class Shader> _shadowShader = { nullptr };

	// 디퍼드렌더링용 버텍스버퍼
	shared_ptr<class VertexBuffer> _vertexBuffer;

private:
	list<shared_ptr<GameObject>> _renderObjects[ETOUI(RENDERGROUP::END)];


public:
	static unique_ptr<Renderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
};

NS_END
