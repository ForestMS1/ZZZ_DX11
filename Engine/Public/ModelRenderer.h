#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class Shader;
class Model;

class ENGINE_DLL ModelRenderer : public Component
{
public:
	ModelRenderer(shared_ptr<Shader> shader);
	virtual ~ModelRenderer();

	// 생명주기 함수
	virtual void Update() override;
	virtual HRESULT Render() override;

	// 그릴 Model Resource를 Set
	void SetModel(shared_ptr<Model> model);
	shared_ptr<Shader> GetShader() { return _shader; }

	shared_ptr<Model> GetModel() { return _model; }

	// Technique Property
	void SetTechnique(uint8 technique) { _techniqueIndex = technique; }
	uint8 GetTechnique() { return _techniqueIndex; }
	
	// Pass Property
	void SetPass(uint8 pass) { _pass = pass; }
	uint8 GetPass() { return _pass; }


	// RenderGroup Property
	RENDERGROUP GetCurRenderGroup() { return _renderGroup; }
	void SetCurRenderGroup(RENDERGROUP eRenderGroup) { _renderGroup = eRenderGroup; }

	//ImGui
	virtual void OnInspectorGUI() override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return nullptr; }

private:
	shared_ptr<Shader> _shader;
	shared_ptr<Model> _model;

	uint8 _techniqueIndex = 0;
	uint8 _pass = 0;

	RENDERGROUP _renderGroup = RENDERGROUP::PRIORITY; // 따로 설정 안했으면 PRIORITY
};

NS_END
