#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL RenderTarget
{
public:
	RenderTarget(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext, Vec4 clearColor);
	virtual ~RenderTarget();

public:
	HRESULT CreateRTVWithSRV(DXGI_FORMAT format, int32 sizeX, int32 sizeY);

	// RenderTargetManager에서 호출
	void ClearRenderTarget() { _deviceContext->ClearRenderTargetView(_rtv.Get(), reinterpret_cast<const float*>(&_clearColor)); }


	ComPtr<ID3D11Texture2D>				GetTexture2D()		{ return _texture2D; }
	ComPtr<ID3D11RenderTargetView>		GetRTV()			{ return _rtv; }
	ComPtr<ID3D11ShaderResourceView>	GetSRV()			{ return _srv; }

	HRESULT Ready_Debug(float x, float y, float sizeX, float sizeY);
	HRESULT RenderRTV(shared_ptr<Shader> shader, uint8 pass = 1);


private:
	ComPtr<ID3D11Device>					_device = { nullptr };
	ComPtr<ID3D11DeviceContext>				_deviceContext = { nullptr };

	ComPtr<ID3D11Texture2D>					_texture2D = { nullptr };
	ComPtr<ID3D11RenderTargetView>			_rtv = { nullptr };
	ComPtr<ID3D11ShaderResourceView>		_srv = { nullptr };

	Vec4									_clearColor = { 0.f, 0.f, 0.f, 0.f };
private:

	Matrix _worldMatrix;
	Matrix _projMatrix;
	float	_x, _y, _sizeX, _sizeY;

	shared_ptr<Geometry<VertexTextureData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;
	uint8 pass = 1;
};

NS_END
