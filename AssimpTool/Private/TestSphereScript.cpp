#include "pch.h"
#include "TestSphereScript.h"
#include "Transform.h"
#include "Model.h"
#include "ModelRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
void TestSphereScript::Start()
{
	shared_ptr<Shader> shader = make_shared<Shader>(L"Test.fx");
	
	shared_ptr<Model> model = make_shared<Model>();
	model->ReadModel(L"Corin/Corin");
	model->ReadMaterial(L"Corin/Corin");
	
	GetGameObject()->AddComponent(make_shared<ModelRenderer>(shader));
	GetGameObject()->GetModelRenderer()->SetModel(model);

	GetTransform()->SetScale(Vec3(1.f));
}

void TestSphereScript::Update()
{
	GAME.Add_RenderObject(RENDERGROUP::NONBLEND, GetGameObject());
}
