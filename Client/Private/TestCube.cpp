#include "pch.h"
#include "TestCube.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "CubeScritp.h"

TestCube::TestCube(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

TestCube::TestCube(const TestCube& Prototype)
	: GameObject(Prototype)
{
}

TestCube::~TestCube()
{
}

HRESULT TestCube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT TestCube::Initialize(void* pArg)
{
	GameObject::Initialize(pArg);
	AddComponent(make_shared<CubeScritp>());

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	shared_ptr<Material> material = make_shared<Material>();
	shared_ptr<Mesh> mesh = make_shared<Mesh>();

	shared_ptr<Shader> shader = GAME.GetResource<Shader>(L"Test.fx");//make_shared<Shader>(L"Test.fx");

	material->SetShader(shader);
	auto texture = GAME.Load<Texture>(L"zzz", L"..\\..\\Resources\\Textures\\zzz.png");

	material->SetDiffuseMap(texture);
	MaterialDesc& desc = material->GetMaterialDesc();
	desc.ambient = Vec4(1.f);
	desc.diffuse = Vec4(1.f);
	desc.specular = Vec4(1.f);
	GAME.AddResource(L"zzz", material);

	//mesh->CreateQuad();
	mesh = GAME.GetResource<Mesh>(L"Cube");

	meshRenderer->SetMaterial(material);
	meshRenderer->SetMesh(mesh);
	meshRenderer->SetPass(0);
	AddComponent(meshRenderer);

	GetTransform()->SetPosition(Vec3(2.f, 0.f, 2.f));
	GetTransform()->SetScale(Vec3(1.f));

	return S_OK;
}

unique_ptr<TestCube> TestCube::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<TestCube>(new TestCube(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TestCube");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> TestCube::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new TestCube(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TestCube");
		return nullptr;
	}

	return pInstance;
}
