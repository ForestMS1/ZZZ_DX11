#pragma once

NS_BEGIN(Engine)

class RenderTarget;
class Shader;

class RenderTargetManager
{
public:
	RenderTargetManager(ComPtr<ID3D11RenderTargetView> backBufferRTV, ComPtr<ID3D11DepthStencilView> depthStencilView);
	~RenderTargetManager();

public:
	// _renderTargets에 추가
	void Add_RenderTarget(const wstring& name, shared_ptr<RenderTarget> renderTarget);
	// _renderTargets에 있는 애를 MRT그룹에 추가
	void Add_RenderTargetToMRT(const wstring& mrtName, const wstring& renderTargetName);

	// MRT그룹을 바인드/언바인드
	void MultiRenderTargetBind(const wstring& mrtName);
	void MultiRenderTargetUnbind();

private:
	shared_ptr<RenderTarget> FindRenderTarget(const wstring renderTargetName);
	list<shared_ptr<RenderTarget>>* FindMRT(const wstring& mrtName);

private:
	map<wstring, shared_ptr<RenderTarget>>			_renderTargets;
	map<wstring, list<shared_ptr<RenderTarget>>>	_multiRenderTarget; // OMSetRenderTargets 해줄 애들 그룹

	ComPtr<ID3D11Device>					_device = { nullptr };
	ComPtr<ID3D11DeviceContext>				_deviceContext = { nullptr };

	ComPtr<ID3D11RenderTargetView> _backBufferRTV;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;

	Vec4 _clearColor = { 0.f, 0.f, 1.f, 1.f };
};

NS_END

