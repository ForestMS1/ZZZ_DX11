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


private:
	HRESULT Ready_SwapChain(HWND hWnd, WINMODE isWindowed, int32_t iWinCX, int32_t iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilView(int32 iWinCX, int32 iWinCY);


private:
	ComPtr<ID3D11Device>	_device = { nullptr };
	ComPtr<ID3D11DeviceContext> _deviceContext = { nullptr };

	ComPtr<IDXGISwapChain>	_swapChain = { nullptr };

	ComPtr<ID3D11RenderTargetView> _backBufferRTV = { nullptr };
	ComPtr<ID3D11DepthStencilView> _depthStencilView = { nullptr };


public:
	static unique_ptr<Graphic_Device> Create(HWND hwnd, WINMODE eWinMode, int32 iWinSizeX, int32 iWinSizeY,
		ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext);

};

NS_END