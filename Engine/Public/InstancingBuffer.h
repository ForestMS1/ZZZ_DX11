#pragma once

NS_BEGIN(Engine)

class VertexBuffer;

struct InstancingData
{
	Matrix world;
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