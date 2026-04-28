#include "pch.h"
#include "RenderTargetManager.h"
#include "RenderTarget.h"
RenderTargetManager::RenderTargetManager(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
	: _device(device), _deviceContext(deviceContext)
{
}

RenderTargetManager::~RenderTargetManager()
{
}

HRESULT RenderTargetManager::Initialize(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
{
	// 컨텍스트로부터 백퍼퍼, 뎁스스텐실 받아와서 저장해둠
	ID3D11RenderTargetView* tempRTV = nullptr;
	ID3D11DepthStencilView* tempDSV = nullptr;

	// 1. RAW 포인터로 받음 (참조 횟수 +1 상태)
	_deviceContext->OMGetRenderTargets(1, &tempRTV, &tempDSV);

	// 2. ComPtr에 Attach (참조 횟수를 그대로 유지하며 소유권 이전)
	_backBufferRTV.Attach(tempRTV);
	_depthStencilView.Attach(tempDSV);
	return S_OK;
}

void RenderTargetManager::Add_RenderTarget(const wstring& name, shared_ptr<RenderTarget> renderTarget)
{
	// 이미 있음
	if (nullptr != FindRenderTarget(name))
		return;

	_renderTargets.emplace(name, renderTarget);
}

void RenderTargetManager::Add_RenderTargetToMRT(const wstring& mrtName, const wstring& renderTargetName)
{
	shared_ptr<RenderTarget> renderTarget = FindRenderTarget(renderTargetName);
	if (nullptr == renderTarget)
		return;

	// operator[]를 사용하면 해당 키가 없을 때 자동으로 빈 list를 생성해줌
	auto& targetList = _multiRenderTarget[mrtName];

	targetList.push_back(renderTarget);
}

void RenderTargetManager::MultiRenderTargetBind(const wstring& mrtName, bool originDepthStencilApply)
{
	list<shared_ptr<RenderTarget>>* pList = FindMRT(mrtName);
	if (pList == nullptr)
		return;

	ID3D11RenderTargetView* bindRenderTargets[8];
	uint8 viewCount = 0;


	for (auto& renderTarget : *pList)
	{
		renderTarget->ClearRenderTarget();

		bindRenderTargets[viewCount++] = renderTarget->GetRTV().Get();
	}

	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	originDepthStencilApply ? _deviceContext->OMSetRenderTargets(viewCount, bindRenderTargets, _depthStencilView.Get()) : _deviceContext->OMSetRenderTargets(viewCount, bindRenderTargets, nullptr);

	// 뷰포트 설정
}

void RenderTargetManager::MultiRenderTargetUnbind()
{
	// 셰이더 리소스와 충돌 방지를 위해 일단 모든 RTV 해제
	ID3D11RenderTargetView* nullRTVs[8] = { nullptr, };
	_deviceContext->OMSetRenderTargets(8, nullRTVs, nullptr);


	// 원래의 백버퍼와 뎁스스텐실로 복구
	_deviceContext->OMSetRenderTargets(1, _backBufferRTV.GetAddressOf(), _depthStencilView.Get());

	// 백버퍼용 뷰포트 재설정 필요
}

HRESULT RenderTargetManager::RenderRTV(const wstring& pMRTTag, shared_ptr<Shader> pShader, uint8 pass)
{
	list<shared_ptr<RenderTarget>>* pMRTList = FindMRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pTarget : *pMRTList)
		pTarget->RenderRTV(pShader, pass);

	return S_OK;
}

shared_ptr<RenderTarget> RenderTargetManager::FindRenderTarget(const wstring& renderTargetName)
{
	auto iter = _renderTargets.find(renderTargetName);
	if (iter == _renderTargets.end())
		return nullptr;
	else
		return iter->second;
}

list<shared_ptr<RenderTarget>>* RenderTargetManager::FindMRT(const wstring& mrtName)
{
	auto iter = _multiRenderTarget.find(mrtName);
	if (iter == _multiRenderTarget.end())
		return nullptr;
	else
		return &(iter->second);
}

HRESULT RenderTargetManager::Ready_Debug(const wstring& pTargetTag, float x, float y, float sizeX, float sizeY)
{
	shared_ptr<RenderTarget> pTarget = FindRenderTarget(pTargetTag);
	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->Ready_Debug(x, y, sizeX, sizeY);
}

unique_ptr<RenderTargetManager> RenderTargetManager::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
{
	auto pInstance = unique_ptr<RenderTargetManager>(new RenderTargetManager(device, deviceContext));

	if (FAILED(pInstance->Initialize(device, deviceContext)))
	{
		MSG_BOX("Failed to Cloned : RenderTargetManager");
		return nullptr;
	}

	return pInstance;
}