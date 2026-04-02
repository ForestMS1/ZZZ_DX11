#pragma once
#include "ResourceBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL Texture final : public ResourceBase
{
public:
	Texture();
	~Texture();

	ComPtr<ID3D11ShaderResourceView> GetComPtr() { return _shaderResourceView; }
	ComPtr<ID3D11Texture2D> GetTexture2D();
	Vec2 GetSize() { return _size; }
	const DirectX::ScratchImage& GetInfo() { return _img; }


	void SetSRV(ComPtr<ID3D11ShaderResourceView> srv) { _shaderResourceView = srv; }

	virtual void Load(const wstring& path) override;


private:
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
	Vec2 _size = { 0.f, 0.f };
	DirectX::ScratchImage _img = {};
};

NS_END
