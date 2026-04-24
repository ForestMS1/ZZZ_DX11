#include "pch.h"
#include "RenderTargetManager.h"
#include "RenderTarget.h"
RenderTargetManager::RenderTargetManager(ComPtr<ID3D11RenderTargetView> backBufferRTV, ComPtr<ID3D11DepthStencilView> depthStencilView)
	: _backBufferRTV(backBufferRTV), _depthStencilView(depthStencilView)
{
}

RenderTargetManager::~RenderTargetManager()
{
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
	list<shared_ptr<RenderTarget>>* pList = FindMRT(mrtName);
	if (nullptr == pList)
		return;
	shared_ptr<RenderTarget> renderTarget = FindRenderTarget(renderTargetName);
	if (nullptr == renderTarget)
		return;

	pList->push_back(renderTarget);
}

void RenderTargetManager::MultiRenderTargetBind(const wstring& mrtName)
{
	list<shared_ptr<RenderTarget>>* pList = FindMRT(mrtName);
	if (pList == nullptr)
		return;

	// 컨텍스트로부터 백퍼퍼, 뎁스스텐실 받아와서 저장해둠 -> 생성자에서
	//_deviceContext->OMGetRenderTargets(1, _backBufferRTV.GetAddressOf(), _depthStencilView.GetAddressOf());


	ID3D11RenderTargetView* bindRenderTargets[8];
	uint8 viewCount = 0;


	for (auto& renderTarget : *pList)
	{
		renderTarget->ClearRenderTarget(&_clearColor);

		bindRenderTargets[viewCount++] = renderTarget->GetRTV().Get();
	}


	_deviceContext->OMSetRenderTargets(viewCount, bindRenderTargets, _depthStencilView.Get());
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

shared_ptr<RenderTarget> RenderTargetManager::FindRenderTarget(const wstring renderTargetName)
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
