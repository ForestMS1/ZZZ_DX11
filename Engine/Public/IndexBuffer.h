#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL IndexBuffer final
{
public:
	IndexBuffer();
	~IndexBuffer();

	ComPtr<ID3D11Buffer> GetComPtr() { return _indexBuffer; }
	uint32 GetStride() const { return _stride; }
	uint32 GetOffset() const { return _offset; }
	uint32 GetCount() const { return _count; }

	void Create(const vector<uint32>& indices);

	void PushData();

private:
	ComPtr<ID3D11Buffer> _indexBuffer = { nullptr };

	uint32 _stride = 0;
	uint32 _offset = 0;
	uint32 _count = 0;
};

NS_END