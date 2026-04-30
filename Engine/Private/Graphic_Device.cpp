#include "pch.h"
#include "Graphic_Device.h"

Graphic_Device::Graphic_Device()
{
}

Graphic_Device::~Graphic_Device()
{
}

HRESULT Graphic_Device::Initialize(HWND hwnd, WINMODE eWinMode, int32 iWinSizeX, int32 iWinSizeY, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext)
{
	int32 iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL FeatureLV;

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0,
		D3D11_SDK_VERSION, &_device, &FeatureLV, &_deviceContext)))
		return E_FAIL;

	if (FAILED(Ready_SwapChain(hwnd, eWinMode, iWinSizeX, iWinSizeY)))
		return E_FAIL;

	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilView(iWinSizeX, iWinSizeY)))
		return E_FAIL;


	ID3D11RenderTargetView* pRTVS[] = {
		_backBufferRTV.Get(),
	};

	_deviceContext->OMSetRenderTargets(1, pRTVS, _depthStencilView.Get());

	//D3D11_VIEWPORT		ViewPortDesc;
	ZeroMemory(&_viewPortDesc, sizeof(D3D11_VIEWPORT));
	_viewPortDesc.TopLeftX = 0;
	_viewPortDesc.TopLeftY = 0;
	_viewPortDesc.Width = (float)iWinSizeX;
	_viewPortDesc.Height = (float)iWinSizeY;
	_viewPortDesc.MinDepth = 0.f;
	_viewPortDesc.MaxDepth = 1.f;

	_deviceContext->RSSetViewports(1, &_viewPortDesc);

	pOutDevice = _device;
	pOutDeviceContext = _deviceContext;

	return S_OK;
}

HRESULT Graphic_Device::Clear_BackBuffer_View(const Vec4* pClearColor)
{
	if (_deviceContext == nullptr)
		return E_FAIL;

	ID3D11RenderTargetView* pRTVS[] = {
		_backBufferRTV.Get(),
	};

	// MRT 바인딩
	_deviceContext->OMSetRenderTargets(1, pRTVS, _depthStencilView.Get());

	// 메인 화면 Clear
	_deviceContext->ClearRenderTargetView(_backBufferRTV.Get(), reinterpret_cast<const float*>(pClearColor));

	return S_OK;
}

HRESULT Graphic_Device::Clear_DepthStencil_View()
{
	if (_deviceContext == nullptr)
		return E_FAIL;

	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT Graphic_Device::Present()
{
	if (_swapChain == nullptr)
		return E_FAIL;

	return _swapChain->Present(0,0);
}

void Graphic_Device::ShutDown()
{
	_deviceContext->ClearState();
	_deviceContext->Flush();


	_swapChain.Reset();
	_backBufferRTV.Reset();
	_depthStencilView.Reset();
	_deviceContext.Reset();


	//#if defined(DEBUG) || defined(_DEBUG)
	//	ID3D11Debug* d3dDebug;
	//	HRESULT hr = _device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	//	if (SUCCEEDED(hr))
	//	{
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//
	//		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	//
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//	}
	//	if (d3dDebug != nullptr)            d3dDebug->Release();
	//#endif


	_device.Reset();
}

void Graphic_Device::SetViewPort(uint32 width, uint32 height)
{
	ZeroMemory(&_viewPortDesc,0, sizeof(_viewPortDesc));
	_viewPortDesc.TopLeftX = 0.f;
	_viewPortDesc.TopLeftY = 0.f;
	_viewPortDesc.Width = (float)width;
	_viewPortDesc.Height = (float)height;
	_viewPortDesc.MinDepth = 0.f;
	_viewPortDesc.MaxDepth = 1.f;

	_deviceContext->RSSetViewports(1, &_viewPortDesc);
}

HRESULT Graphic_Device::Ready_SwapChain(HWND hWnd, WINMODE isWindowed, int32_t iWinCX, int32_t iWinCY)
{
	ComPtr<IDXGIDevice>		pDevice = nullptr;
	_device->QueryInterface(__uuidof(IDXGIDevice), (void**)pDevice.GetAddressOf());

	ComPtr<IDXGIAdapter>	pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf());

	ComPtr<IDXGIFactory>	pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf());

	DXGI_SWAP_CHAIN_DESC	SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	SwapChain.BufferDesc.Width = iWinCX;
	SwapChain.BufferDesc.Height = iWinCY;

	SwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 1;

	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;

	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;

	SwapChain.OutputWindow = hWnd;
	SwapChain.Windowed = static_cast<BOOL>(isWindowed);
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(pFactory->CreateSwapChain(_device.Get(), &SwapChain, _swapChain.GetAddressOf())))
		return E_FAIL;


	return S_OK;
}

HRESULT Graphic_Device::Ready_BackBufferRenderTargetView()
{
	if (_device == nullptr)
		return E_FAIL;

	ComPtr<ID3D11Texture2D>		pBackBufferTexture = nullptr;

	if (FAILED(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pBackBufferTexture.GetAddressOf())))
		return E_FAIL;

	if (FAILED(_device->CreateRenderTargetView(pBackBufferTexture.Get(), nullptr, _backBufferRTV.GetAddressOf())))
		return E_FAIL;


	return S_OK;
}

HRESULT Graphic_Device::Ready_DepthStencilView(int32 iWinCX, int32 iWinCY)
{
	if (_device == nullptr)
		return E_FAIL;

	D3D11_TEXTURE2D_DESC	TextureDesc{};
	ZeroMemory(&TextureDesc, sizeof(TextureDesc));
	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // R32_TYPLESS는 나중에 DSV로도, SRV로도 쓸 수 있는 유연한 포맷입니다.

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;

	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(_device->CreateTexture2D(&TextureDesc, nullptr, _depthTexture.GetAddressOf())))
		return E_FAIL;

	// 깊이 테스트용 뷰 (DSV)
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // 실제 깊이 값은 float으로 처리
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	if (FAILED(_device->CreateDepthStencilView(_depthTexture.Get(), &dsvDesc, _depthStencilView.GetAddressOf())))
		return E_FAIL;

	// 셰이더에서 읽기용 뷰 (SRV)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1; // 모든 밉 레벨 접근

	if(FAILED(_device->CreateShaderResourceView(_depthTexture.Get(), &srvDesc, _depthSRV.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

unique_ptr<Graphic_Device> Graphic_Device::Create(HWND hwnd, WINMODE eWinMode, int32 iWinSizeX, int32 iWinSizeY, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext)
{
	auto pInstance = unique_ptr<Graphic_Device>(new Graphic_Device); // 생성자가 private이어서 make_unique 사용 불가 

	if (FAILED(pInstance->Initialize(hwnd, eWinMode, iWinSizeX, iWinSizeY, pOutDevice, pOutDeviceContext)))
	{
		MSG_BOX("Failed to Created : CGraphic_Device");
		return nullptr;
	}
	return pInstance;
}
