#pragma once
#include "Component.h"


NS_BEGIN(Engine)

class ENGINE_DLL MeshRenderer : public Component
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();
public:
	// Mesh, Material Set
	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	shared_ptr<Mesh> GetMesh() { return _mesh; }

	void SetMaterial(shared_ptr<Material> material) { _material = material; }
	shared_ptr<Material> GetMaterial() { return _material; }

	void SetPass(uint8 pass) { _pass = pass; }
	uint8 GetPass() { return _pass; }

	void SetTechnique(uint8 technique) { _techniqueIndex = technique; }
	uint8 GetTechnique() { return _techniqueIndex; }

	// 생명주기 함수
	virtual void Awake() override;
	virtual void Update() override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;

	// RenderGroup Property
	RENDERGROUP GetCurRenderGroup() { return _renderGroup; }
	void SetCurRenderGroup(RENDERGROUP eRenderGroup) { _renderGroup = eRenderGroup; }

	//void RenderInstancing(shared_ptr<class InstancingBuffer>& buffer);
	//InstanceID GetInstanceID();

	//ImGui용
	virtual void OnInspectorGUI() override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return nullptr; }
private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;
	shared_ptr<Shader> _shadowShader;

	uint8 _techniqueIndex = 0;
	uint8 _pass = 0;

	RENDERGROUP _renderGroup = RENDERGROUP::PRIORITY; // 따로 설정 안했으면 PRIORITY
};

NS_END