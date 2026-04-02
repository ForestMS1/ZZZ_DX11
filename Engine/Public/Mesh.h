#pragma once
#include "ResourceBase.h"
#include "Geometry.h"
NS_BEGIN(Engine)

class VertexBuffer;
class IndexBuffer;

class ENGINE_DLL Mesh : public ResourceBase
{
public:
	Mesh();
	virtual ~Mesh();

	void CreateQuad();
	void CreateCube();
	void CreateGrid(int32 sizeX, int32 sizeZ);
	void CreateSphere();


	shared_ptr<Geometry<VertexTextureNormalTangentData>> GetGeometry() { return _geometry; }
	shared_ptr<VertexBuffer> GetVertexBuffer() { return _vertexBuffer; }
	shared_ptr<IndexBuffer> GetIndexBuffer() { return _indexBuffer; }

private:
	void CreateBuffers();

private:
	shared_ptr<Geometry<VertexTextureNormalTangentData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;
};

NS_END
