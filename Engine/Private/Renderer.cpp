#include "pch.h"
#include "Renderer.h"
#include "GameObject.h"

Renderer::Renderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: _device(pDevice)
	, _deviceContext(pDeviceContext)
{
}

Renderer::~Renderer()
{
}

HRESULT Renderer::Initialize()
{
	// 디퓨즈 노말 깊이 렌더타겟 MRT로 집어넣기
	return S_OK;
}

HRESULT Renderer::Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<GameObject> pRenderObject)
{
	if (eRenderGroup >= RENDERGROUP::END ||
		pRenderObject == nullptr)
		return E_FAIL;

	_renderObjects[ETOUI(eRenderGroup)].push_back(pRenderObject);

	return S_OK;
}

HRESULT Renderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

	return S_OK;
}

HRESULT Renderer::Render_Priority()
{
	for (auto& object : _renderObjects[ETOUI(RENDERGROUP::PRIORITY)])
	{
		if(object != nullptr)
			object->Render();
	}

	_renderObjects[ETOUI(RENDERGROUP::PRIORITY)].clear();

	return S_OK;
}

HRESULT Renderer::Render_NonBlend()
{
	for (auto& object : _renderObjects[ETOUI(RENDERGROUP::NONBLEND)])
	{
		if (object != nullptr)
			object->Render();
	}

	_renderObjects[ETOUI(RENDERGROUP::NONBLEND)].clear();

	return S_OK;
}

HRESULT Renderer::Render_Blend()
{
	for (auto& object : _renderObjects[ETOUI(RENDERGROUP::BLEND)])
	{
		if (object != nullptr)
			object->Render();
	}

	_renderObjects[ETOUI(RENDERGROUP::BLEND)].clear();

	return S_OK;
}

HRESULT Renderer::Render_UI()
{
	for (auto& object : _renderObjects[ETOUI(RENDERGROUP::UI)])
	{
		if (object != nullptr)
			object->Render();
	}

	_renderObjects[ETOUI(RENDERGROUP::UI)].clear();

	return S_OK;
}

unique_ptr<Renderer> Renderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<Renderer>(new Renderer(pDevice, pDeviceContext));
	
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Cloned : Renderer");
		return nullptr;
	}

	return pInstance;
}

