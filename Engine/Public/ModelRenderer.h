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

	virtual HRESULT Render() override;

	void SetModel(shared_ptr<Model> model);
	void SetPass(uint8 pass) { _pass = pass; }

	//ImGui
	void OnInspectorGUI();

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return nullptr; }

private:
	shared_ptr<Shader> _shader;
	shared_ptr<Model> _model;

	uint8 _techniqueIndex = 0;
	uint8 _pass = 0;
};

NS_END
