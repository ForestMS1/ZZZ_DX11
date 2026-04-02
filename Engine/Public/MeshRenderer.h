#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class Mesh;
class Shader;
class Material;

class ENGINE_DLL MeshRenderer : public Component
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }
	void SetPass(uint8 pass) { _pass = pass; }

	//virtual void Update() override;

	//void RenderInstancing(shared_ptr<class InstancingBuffer>& buffer);
	//InstanceID GetInstanceID();

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;
	uint8 _pass = 0;


};

NS_END