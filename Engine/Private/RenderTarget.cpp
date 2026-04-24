#include "pch.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
	: _device(device), _deviceContext(deviceContext)
{
}

RenderTarget::~RenderTarget()
{
}

HRESULT RenderTarget::CreateRTVWithSRV(DXGI_FORMAT format, int32 sizeX, int32 sizeY)
{
	//if (_device == nullptr)
	//	return E_FAIL;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = sizeX;
	desc.Height = sizeY;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;//DXGI_FORMAT_R16G16B16A16_FLOAT;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;


	if (FAILED(_device->CreateTexture2D(&desc, nullptr, _texture2D.GetAddressOf())))
		return E_FAIL;

	if (FAILED(_device->CreateRenderTargetView(_texture2D.Get(), nullptr, _rtv.GetAddressOf())))
		return E_FAIL;

	if (FAILED(_device->CreateShaderResourceView(_texture2D.Get(), nullptr, _srv.GetAddressOf())))
		return E_FAIL;


	return S_OK;
}