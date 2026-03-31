#include "pch.h"
#include "IndexBuffer.h"
#include "GameInstance.h"

IndexBuffer::IndexBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
}

HRESULT IndexBuffer::CreateBuffer(const vector<uint32>& indices)
{
	_stride = sizeof(uint32);
	_count = static_cast<uint32>(indices.size());

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = static_cast<uint32>(_stride * _count);

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = indices.data();

	if (FAILED(GAME.GetDevice()->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf())))
	{
		MSG_BOX("IndexBuffer Create Failed");
		return E_FAIL;
	}

	return S_OK;
}

void IndexBuffer::PushData()
{
	GAME.GetDeviceContext()->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}