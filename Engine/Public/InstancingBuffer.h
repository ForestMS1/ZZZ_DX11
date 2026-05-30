#pragma once

NS_BEGIN(Engine)

class VertexBuffer;

struct InstancingData
{
	Matrix world;
	// X: Texture Array Index (0~9)
	// Y: Alpha Value (시간이 지나면 서서히 사라지게 만들기 위함)
	// Z, W: 여분 변수
	Vec4   etcInfo;
};

#define MAX_MESH_INSTANCE 500

class ENGINE_DLL InstancingBuffer
{
public:
	InstancingBuffer();
	~InstancingBuffer();

public:
	void ClearData();
	void AddData(const InstancingData& data);

	void PushData();

public:
	uint32						GetCount() { return static_cast<uint32>(_data.size()); }
	shared_ptr<VertexBuffer>	GetBuffer() { return _instanceBuffer; }

	void	SetID(uint64 instanceId) { _instanceId = instanceId; }
	uint64	GetID() { return _instanceId; }

private:
	void CreateBuffer(uint64 maxCount = MAX_MESH_INSTANCE);

private:
	uint64						_instanceId = 0;
	shared_ptr<VertexBuffer>	_instanceBuffer;
	uint32						_maxCount = 0;
	vector<InstancingData>		_data;
};

NS_END