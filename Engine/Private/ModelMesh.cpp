#include "pch.h"
#include "ModelMesh.h"
#include "Geometry.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
void ModelMesh::CreateBuffers()
{
	vertexBuffer = make_shared<VertexBuffer>();
	vertexBuffer->Create(geometry->GetVertices());
	indexBuffer = make_shared<IndexBuffer>();
	indexBuffer->Create(geometry->GetIndices());
}