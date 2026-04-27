#include "pch.h"
#include "RenderTarget.h"
#include "GeometryHelper.h"
#include "Camera.h"
RenderTarget::RenderTarget(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext, Vec4 clearColor)
	: _device(device), _deviceContext(deviceContext), _clearColor(clearColor)
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

HRESULT RenderTarget::Ready_Debug(float x, float y, float sizeX, float sizeY)
{
	_x = x;
	_y = y;
	_sizeX = sizeX;
	_sizeY = sizeY;

	uint32 iViewPortCount = 1;
	D3D11_VIEWPORT ViewPortDesc;
	_deviceContext->RSGetViewports(&iViewPortCount, &ViewPortDesc);

	// ÁÂÇ¥ º¸Á¤ ¿¹½Ã (x, y´Â È­¸é»óÀÇ ÇÈ¼¿ ÁÂÇ¥)
	float viewX = _x - (ViewPortDesc.Width * 0.5f) + (_sizeX * 0.5f);
	float viewY = -_y + (ViewPortDesc.Height * 0.5f) - (_sizeY * 0.5f);

	_worldMatrix = Matrix::CreateScale(_sizeX, _sizeY, 1.f) * Matrix::CreateTranslation(viewX, viewY, 0.f);
	_projMatrix = Matrix::CreateOrthographic(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f);
	if (_geometry == nullptr)
	{
		_geometry = make_shared<Geometry<VertexTextureData>>();
		GeometryHelper::CreateQuad(_geometry);
		_vertexBuffer = make_shared<VertexBuffer>();
		_vertexBuffer->Create(_geometry->GetVertices());
		_indexBuffer = make_shared<IndexBuffer>();
		_indexBuffer->Create(_geometry->GetIndices());
	}
	return S_OK;
}

HRESULT RenderTarget::RenderRTV(shared_ptr<Shader> shader, uint8 pass)
{
	shader->PushGlobalData(Matrix::Identity, _projMatrix);
	shader->PushTransformData(TransformDesc{ _worldMatrix });

	shader->GetSRV("RenderTargetUI")->SetResource(_srv.Get());

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_vertexBuffer->PushData();
	_indexBuffer->PushData();
	shader->DrawIndexed(0, pass, _indexBuffer->GetCount(), 0, 0);

	return S_OK;
}
