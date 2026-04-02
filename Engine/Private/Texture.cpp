#include "pch.h"
#include "Texture.h"
#include "GameInstance.h"
Texture::Texture() : ResourceBase(ResourceType::TEXTURE)
{

}

Texture::~Texture()
{

}

ComPtr<ID3D11Texture2D> Texture::GetTexture2D()
{
	ComPtr<ID3D11Texture2D> texture;
	_shaderResourceView->GetResource((ID3D11Resource**)texture.GetAddressOf());
	return texture;
}

void Texture::Load(const wstring& path)
{
	DirectX::TexMetadata md;
	HRESULT hr = LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &md, _img);
	CHECK(hr);

	hr = CreateShaderResourceView(DEVICE.Get(), _img.GetImages(), _img.GetImageCount(), md, _shaderResourceView.GetAddressOf());
	CHECK(hr);

	_size.x = md.width;
	_size.y = md.height;
}
