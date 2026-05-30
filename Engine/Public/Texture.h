#pragma once
#include "ResourceBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL Texture final : public ResourceBase
{
public:
	Texture();
	~Texture();

public:
	// SRV Get
	ComPtr<ID3D11ShaderResourceView> GetComPtr() { return _shaderResourceView; }
	// Texture Get
	ComPtr<ID3D11Texture2D> GetTexture2D();
	Vec2 GetSize() { return _size; }
	// Image Info
	const DirectX::ScratchImage& GetInfo() { return _img; }


	void SetSRV(ComPtr<ID3D11ShaderResourceView> srv) { _shaderResourceView = srv; }

	// Texture 파일 Load 후 Create SRV
	virtual void Load(const wstring& path) override;

	void LoadTexture2DArray(const vector<wstring>& paths);


private:
	// 쉐이더에 넘겨줄 SRV
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
	Vec2 _size = { 0.f, 0.f };
	DirectX::ScratchImage _img = {};
};

NS_END
