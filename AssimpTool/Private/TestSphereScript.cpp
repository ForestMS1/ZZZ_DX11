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
	model->ReadModel(L"Alice/Alice");
	model->ReadMaterial(L"Alice/Alice");
	
	GetGameObject()->AddComponent(make_shared<ModelRenderer>(shader));
	GetGameObject()->GetModelRenderer()->SetModel(model);
	
	GetTransform()->SetPosition(Vec3(0, 0, 0));
	GetTransform()->SetScale(Vec3(1.f));
}

void TestSphereScript::Update()
{
	GAME.Add_RenderObject(RENDERGROUP::NONBLEND, GetGameObject());
}
