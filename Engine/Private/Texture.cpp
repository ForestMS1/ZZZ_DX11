#include "pch.h"
#include "Texture.h"
#include "GameInstance.h"
#include <filesystem>
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

	HRESULT hr;
	// 확장자 구분
	wstring ext = filesystem::path(path).extension().wstring();

	if (ext == L".dds")
	{
		hr = LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, &md, _img);
		CHECK(hr);
	}
	else
	{
		hr = LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &md, _img);
		CHECK(hr);
	}

	hr = CreateShaderResourceView(DEVICE.Get(), _img.GetImages(), _img.GetImageCount(), md, _shaderResourceView.GetAddressOf());
	CHECK(hr);

	_size.x = md.width;
	_size.y = md.height;
}

void Texture::LoadTexture2DArray(const vector<wstring>& paths)
{
	if (paths.empty())
		return;

	// 첫 번째 이미지 로드해서 메타데이터 확보
	DirectX::TexMetadata firstMd;
	DirectX::ScratchImage firstImg;
	HRESULT hr = LoadFromWICFile(paths[0].c_str(), WIC_FLAGS_NONE, &firstMd, firstImg);
	CHECK(hr);

	_size.x = static_cast<float>(firstMd.width);
	_size.y = static_cast<float>(firstMd.height);

	// 루프를 돌며 모든 이미지를 CPU 메모리(ScratchImage)에 먼저 미리 로드
	vector<DirectX::ScratchImage> loadedImages(paths.size());
	vector<D3D11_SUBRESOURCE_DATA> initData(paths.size());

	for (UINT i = 0; i < paths.size(); ++i)
	{
		DirectX::TexMetadata md;
		hr = ::LoadFromWICFile(paths[i].c_str(), WIC_FLAGS_NONE, &md, loadedImages[i]);
		if (FAILED(hr)) continue;

		// 각 슬라이스 사물함 칸에 들어갈 초기 데이터 포인터 연결
		const DirectX::Image* srcImage = loadedImages[i].GetImage(0, 0, 0);
		if (srcImage == nullptr) continue;

		initData[i].pSysMem = srcImage->pixels;
		initData[i].SysMemPitch = static_cast<UINT>(srcImage->rowPitch);
		initData[i].SysMemSlicePitch = static_cast<UINT>(srcImage->slicePitch);
	}

	// Texture2DArray Desc 설정
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = static_cast<UINT>(firstMd.width);
	texDesc.Height = static_cast<UINT>(firstMd.height);
	texDesc.MipLevels = 1;
	texDesc.ArraySize = static_cast<UINT>(paths.size());
	texDesc.Format = firstMd.format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// CreateTexture2D 할 때 initData 배열을 통째로 밀어넣음
	ComPtr<ID3D11Texture2D> textureArray;
	hr = DEVICE->CreateTexture2D(&texDesc, initData.data(), textureArray.GetAddressOf());
	CHECK(hr);

	// SRV 생성 및 바인딩
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = texDesc.ArraySize;

	hr = DEVICE->CreateShaderResourceView(textureArray.Get(), &srvDesc, _shaderResourceView.GetAddressOf());
	CHECK(hr);
}
