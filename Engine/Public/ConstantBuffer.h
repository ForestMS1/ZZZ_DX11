#pragma once

#include "GameInstance.h"

NS_BEGIN(Engine)

template<typename T>
class ENGINE_DLL ConstantBuffer
{
public:
	ConstantBuffer() = default;
	~ConstantBuffer() = default;

public:
	ComPtr<ID3D11Buffer> GetComPtr() { return _constantBuffer; }

	void Create()
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC; // cpu wrtie + gpu read
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(T);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = DEVICE->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
		CHECK(hr);
	}

	void CopyData(const T& data)
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		ZeroMemory(&subResource, sizeof(subResource));

		CONTEXT->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		::memcpy(subResource.pData, &data, sizeof(data));
		CONTEXT->Unmap(_constantBuffer.Get(), 0);
	}

private:
	ComPtr<ID3D11Buffer> _constantBuffer = { nullptr };
};

NS_END
