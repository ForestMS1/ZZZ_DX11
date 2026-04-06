#include "pch.h"
#include "SkyBoxScript.h"
#include "Texture.h"

void SkyBoxScript::Start()
{
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	shared_ptr<Material> material = make_shared<Material>();
	shared_ptr<Mesh> mesh = make_shared<Mesh>();

	shared_ptr<Shader> shader = make_shared<Shader>(L"SkyBox.fx");

	material->SetShader(shader);
	auto texture = GAME.Load<Texture>(L"Sky", L"..\\..\\Resources\\Textures\\Sky.jpg");

	material->SetDiffuseMap(texture);
	MaterialDesc& desc = material->GetMaterialDesc();
	desc.ambient = Vec4(1.f);
	desc.diffuse = Vec4(1.f);
	desc.specular = Vec4(1.f);
	GAME.AddResource(L"Sky", material);

	//mesh->CreateQuad();
	mesh = GAME.GetResource<Mesh>(L"Sphere");

	meshRenderer->SetMaterial(material);
	meshRenderer->SetMesh(mesh);
	meshRenderer->SetPass(0);
	GetGameObject()->AddComponent(meshRenderer);
}

void SkyBoxScript::Update()
{
	GAME.Add_RenderObject(RENDERGROUP::PRIORITY, GetGameObject());
}
