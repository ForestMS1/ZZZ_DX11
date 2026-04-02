#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class Mesh;
class Shader;
class Material;

#define MAX_MESH_INSTANCE 500

class ENGINE_DLL MeshRenderer : public Component
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }
	void SetPass(uint8 pass) { _pass = pass; }

	virtual HRESULT Render() override;

	//void RenderInstancing(shared_ptr<class InstancingBuffer>& buffer);
	//InstanceID GetInstanceID();

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return nullptr; }
private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;
	uint8 _pass = 0;


};

NS_END