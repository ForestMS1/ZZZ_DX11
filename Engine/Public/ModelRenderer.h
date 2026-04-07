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

	virtual void Update() override;
	virtual HRESULT Render() override;

	void SetModel(shared_ptr<Model> model);
	void SetPass(uint8 pass) { _pass = pass; }

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
