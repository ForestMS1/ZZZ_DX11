#pragma once

NS_BEGIN(Engine)

class Graphic_Device final
{
private:
	Graphic_Device();
public:
	~Graphic_Device();

public:
	HRESULT Initialize(HWND hwnd, WINMODE eWinMode, int32 iWinSizeX, int32 iWinSizeY,
		ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext);

	HRESULT Clear_BackBuffer_View(const Vec4* pClearColor);

	HRESULT Clear_DepthStencil_View();

	HRESULT Present();

	void ShutDown();

	ComPtr<ID3D11Device> GetDevice() { return _device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _deviceContext; }
	ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }

	ComPtr<ID3D11RenderTargetView> GetBackRTV() { return _backBufferRTV; }
	// SRV
	ComPtr<ID3D11ShaderResourceView> GetNormalSRV() { return _normalSRV; }
	ComPtr<ID3D11ShaderResourceView> GetSpecularSRV() { return _specularSRV; }
	ComPtr<ID3D11ShaderResourceView> GetDepthSRV() { return _depthSRV; }


private:
	HRESULT Ready_SwapChain(HWND hWnd, WINMODE isWindowed, int32_t iWinCX, int32_t iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilView(int32 iWinCX, int32 iWinCY);

	// MRT
	HRESULT Ready_NormalRenderTargetView(int32 iWinCX, int32 iWinCY);
	HRESULT Ready_SpecularRenderTargetView(int32 iWinCX, int32 iWinCY);


private:
	ComPtr<ID3D11Device>	_device = { nullptr };
	ComPtr<ID3D11DeviceContext> _deviceContext = { nullptr };

	ComPtr<IDXGISwapChain>	_swapChain = { nullptr };

	ComPtr<ID3D11RenderTargetView> _backBufferRTV = { nullptr };

	ComPtr<ID3D11Texture2D> _depthTexture = { nullptr };
	ComPtr<ID3D11DepthStencilView> _depthStencilView = { nullptr };
	// 깊이 정보 쉐이더에서 쓸 수 있게 SRV 추가
	ComPtr<ID3D11ShaderResourceView> _depthSRV = { nullptr };

	ComPtr<ID3D11Texture2D> _normalTexture = { nullptr };
	ComPtr<ID3D11RenderTargetView> _normalRTV = { nullptr };
	ComPtr<ID3D11ShaderResourceView> _normalSRV = { nullptr };

	ComPtr<ID3D11Texture2D> _specularTexture = { nullptr };
	ComPtr<ID3D11RenderTargetView> _specularRTV = { nullptr };
	ComPtr<ID3D11ShaderResourceView> _specularSRV = { nullptr };


public:
	static unique_ptr<Graphic_Device> Create(HWND hwnd, WINMODE eWinMode, int32 iWinSizeX, int32 iWinSizeY,
		ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext);

};

NS_END