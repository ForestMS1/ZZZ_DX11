#include "pch.h"
#include "Renderer.h"
#include "GameObject.h"

#include "RenderTarget.h"
#include "Shader.h"
#include "GeometryHelper.h"
#include "Geometry.h"
#include "VertexBuffer.h"
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
	const ENGINE_DESC& desc = GAME.GetEngineDesc();

	// 디퓨즈 노말 깊이 렌더타겟 MRT로 집어넣기
	shared_ptr<RenderTarget> diffuseRenderTarget = make_shared<RenderTarget>(_device, _deviceContext, Vec4(0.f, 0.f, 0.f, 0.f));
	diffuseRenderTarget->CreateRTVWithSRV(DXGI_FORMAT_B8G8R8A8_UNORM, desc.iWinSizeX, desc.iWinSizeY);
	GAME.Add_RenderTarget(L"Target_Diffuse", diffuseRenderTarget);

	shared_ptr<RenderTarget> normalRenderTarget = make_shared<RenderTarget>(_device, _deviceContext, Vec4(0.5f, 0.5f, 1.f, 1.f));
	normalRenderTarget->CreateRTVWithSRV(DXGI_FORMAT_R16G16B16A16_UNORM, desc.iWinSizeX, desc.iWinSizeY);
	GAME.Add_RenderTarget(L"Target_Normal", normalRenderTarget);

	shared_ptr<RenderTarget> depthRenderTarget = make_shared<RenderTarget>(_device, _deviceContext, Vec4(1.f));
	depthRenderTarget->CreateRTVWithSRV(DXGI_FORMAT_R32G32B32A32_FLOAT, desc.iWinSizeX, desc.iWinSizeY);
	GAME.Add_RenderTarget(L"Target_World", depthRenderTarget);

	// 보통 그림자 맵은 퀄리티를 위해 해상도를 높게 잡음 (2048x2048)
	shared_ptr<RenderTarget> shadowTarget = make_shared<RenderTarget>(_device, _deviceContext, Vec4(1.f, 1.f, 1.f, 1.f));
	shadowTarget->CreateRTVWithSRV(DXGI_FORMAT_R32_FLOAT, 2048.f, 2048.f); // 깊이만 저장하므로 R32_FLOAT
	GAME.Add_RenderTarget(L"Target_Shadow", shadowTarget);

	//_shadowShader = Shader::Create(L"Shadow.fx"); // 그림자 기록용 셰이더

	GAME.Add_RenderTargetToMRT(L"MRT_Deferred", L"Target_Diffuse");
	GAME.Add_RenderTargetToMRT(L"MRT_Deferred", L"Target_Normal");
	GAME.Add_RenderTargetToMRT(L"MRT_Deferred", L"Target_World");

	GAME.Add_RenderTargetToMRT(L"MRT_Shadow", L"Target_Shadow");


	_finalBindShader = Shader::Create(L"FinalBind.fx");
	_renderTargetShader = Shader::Create(L"UI.fx");

	auto geometry = make_shared<Geometry<VertexTextureData>>();
	GeometryHelper::CreateQuad(geometry);
	_vertexBuffer = make_shared<VertexBuffer>();
	_vertexBuffer->Create(geometry->GetVertices());

	if (FAILED(GAME.Ready_Debug(L"Target_Diffuse", 0.f, 0.f, 200.f, 150.f)))
		return E_FAIL;
	if (FAILED(GAME.Ready_Debug(L"Target_Normal", 0.f, 150.f, 200.f, 150.f)))
		return E_FAIL;
	if (FAILED(GAME.Ready_Debug(L"Target_World", 0.f, 300.f, 200.f, 150.f)))
		return E_FAIL;

	if (FAILED(GAME.Ready_Debug(L"Target_Shadow", 0.f, 450.f, 200.f, 150.f)))
		return E_FAIL;


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
	// DSV를 nullptr로!
	GAME.MultiRenderTargetBind(L"MRT_Shadow", false);

	// 그림자 기록 텍스쳐만큼 뷰포트 사이즈 변경
	GAME.SetViewPort(2048, 2048);

	if (FAILED(Render_Shadow()))
		return E_FAIL;

	// 다시 원래 뷰포트 사이즈로 복원
	GAME.SetViewPort(GAME.GetEngineDesc().iWinSizeX, GAME.GetEngineDesc().iWinSizeY);

	GAME.MultiRenderTargetBind(L"MRT_Deferred");

	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Redner_Collider()))
		return E_FAIL;
#endif
	// 기존 백버퍼와, DSV로 되돌림
	GAME.MultiRenderTargetUnbind();

#ifdef _DEBUG
//	GAME.RenderRTV(L"MRT_Deferred", _renderTargetShader, 1);
//	GAME.RenderRTV(L"MRT_Shadow", _renderTargetShader, 1);
#endif

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

	// 모든 텍스쳐들을 합쳐서 한 장면으로
	if (FAILED(Render_Deferred_Lighting()))
		return E_FAIL;


	return S_OK;
}

HRESULT Renderer::Render_Shadow()
{
	for (auto& object : _renderObjects[ETOUI(RENDERGROUP::PRIORITY)])
	{
		if (object != nullptr)
			object->RenderShadow();
	}
	for (auto& object : _renderObjects[ETOUI(RENDERGROUP::NONBLEND)])
	{
		if (object != nullptr)
			object->RenderShadow();
	}
	return S_OK;
}

HRESULT Renderer::Render_Priority()
{
	for (auto& object : _renderObjects[ETOUI(RENDERGROUP::PRIORITY)])
	{
		if (object != nullptr)
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
	auto& uiObjects = _renderObjects[ETOUI(RENDERGROUP::UI)];

	uiObjects.sort([](const auto& a, const auto& b)
		{
			auto aSpriteRenderer = a->GetSpriteRenderer();
			auto bSpriteRenderer = b->GetSpriteRenderer();

			if (aSpriteRenderer == nullptr || bSpriteRenderer == nullptr)
				return false;

			return aSpriteRenderer->GetZOrder() < bSpriteRenderer->GetZOrder();
		});



	for (auto& object : _renderObjects[ETOUI(RENDERGROUP::UI)])
	{
		if (object != nullptr)
			object->Render();
	}

	_renderObjects[ETOUI(RENDERGROUP::UI)].clear();

	return S_OK;
}

HRESULT Renderer::Redner_Collider()
{
	for (auto& object : _renderObjects[ETOUI(RENDERGROUP::COLLIDER)])
	{
		if (object != nullptr)
		{
			if (auto collider = object->GetCollider())
			{
				if(collider->IsActive())
					collider->Render();
			}
		}
	}

	_renderObjects[ETOUI(RENDERGROUP::COLLIDER)].clear();

	return S_OK;
}

HRESULT Renderer::Render_Deferred_Lighting()
{
	// Unbind 에서 백버퍼를 다시 출력 타겟으로 잡았음


	// MRT 결과물들을 셰이더의 SRV 슬롯에 세팅
	// (Target_Diffuse, Target_Normal, Target_Depth 등)
	auto diffuse = GAME.FindRenderTarget(L"Target_Diffuse")->GetSRV();
	auto normal = GAME.FindRenderTarget(L"Target_Normal")->GetSRV();
	auto world = GAME.FindRenderTarget(L"Target_World")->GetSRV();
	auto shadowDepth = GAME.FindRenderTarget(L"Target_Shadow")->GetSRV();

	auto& lightList = GAME.GetLigthList();
	if (!lightList.empty())
	{
		auto& lightDesc = lightList.front()->GetLightDesc();
		_finalBindShader->PushLightData(lightDesc);
	
		const Matrix& lightView = lightList.front()->GetLighViewMatrix();
		const Matrix& lightProj = lightList.front()->GetLighProjMatrix();
		_finalBindShader->GetMatrix("g_LightView")->SetMatrix((float*)&lightView);
		_finalBindShader->GetMatrix("g_LightProj")->SetMatrix((float*)&lightProj);
	}

	// 셰이더에 텍스처 전달
	_finalBindShader->GetSRV("g_AlbedoTex")->SetResource(diffuse.Get());
	_finalBindShader->GetSRV("g_NormalTex")->SetResource(normal.Get());
	_finalBindShader->GetSRV("g_WorldTex")->SetResource(world.Get());
	_finalBindShader->GetSRV("g_ShadowTex")->SetResource(shadowDepth.Get());


	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_vertexBuffer->PushData();

	_finalBindShader->Draw(0, 0, _vertexBuffer->GetCount(), 0);


	// 사용한 SRV 해제 (중요: 다음 프레임에 다시 RTV로 써야 하므로)
	ID3D11ShaderResourceView* nullSRVs[4] = { nullptr, nullptr, nullptr, nullptr };
	// 픽셀 셰이더뿐만 아니라 모든 단계의 슬롯을 비워주는 것이 좋습니다.
	_deviceContext->VSSetShaderResources(0, 4, nullSRVs);
	_deviceContext->PSSetShaderResources(0, 4, nullSRVs);
	_deviceContext->GSSetShaderResources(0, 4, nullSRVs);

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

