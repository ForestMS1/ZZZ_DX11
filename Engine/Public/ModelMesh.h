#pragma once
#include "Geometry.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
NS_BEGIN(Engine)


struct ENGINE_DLL ModelBone
{
	wstring name;
	int32 index;
	int32 parentIndex;
	// 순환참조 문제로 메모리 누수 발생
	// shared_ptr -> weak_ptr
	weak_ptr<ModelBone> parent; // Cache
	
	Matrix transform;
	vector<shared_ptr<ModelBone>> children; // Cache
};

struct ENGINE_DLL ModelMesh
{

	void CreateBuffers();

	wstring name;

	//mesh
	shared_ptr<Geometry<ModelVertexType>> geometry = make_shared<Geometry<ModelVertexType>>();
	shared_ptr<VertexBuffer> vertexBuffer;
	shared_ptr<IndexBuffer> indexBuffer;

	//Material
	wstring materialName = L"";
	shared_ptr<Material> material; // Cache

	//Bones
	int32 boneIndex;
	shared_ptr<ModelBone> bone; // Cache
};




NS_END