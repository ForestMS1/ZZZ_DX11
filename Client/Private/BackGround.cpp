#include "pch.h"
#include "BackGround.h"
#include "BackGroundTestScript.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
BackGround::BackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

BackGround::BackGround(const BackGround& Prototype)
	: GameObject(Prototype)
{
}

BackGround::~BackGround()
{
}

HRESULT BackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT BackGround::Initialize(void* pArg)
{
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	GameObject::Initialize(pArg);
	AddComponent(make_shared<BackGroundTestScript>());
	
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	shared_ptr<Material> material = make_shared<Material>();
	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	
	shared_ptr<Shader> shader = GAME.GetResource<Shader>(L"Test.fx"); ///make_shared<Shader>(L"Test.fx");

	material->SetShader(shader);
	auto texture = GAME.Load<Texture>(L"Sky", L"..\\..\\Resources\\Textures\\Sky01.jpg");
	
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
	AddComponent(meshRenderer);

	GetTransform()->SetPosition(Vec3(0, 0, 0));
	GetTransform()->SetScale(Vec3(1.f));

	return S_OK;
}

//HRESULT BackGround::Render()
//{
//	return S_OK;
//}

unique_ptr<BackGround> BackGround::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<BackGround>(new BackGround(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : BackGround");
		return nullptr;
	}
	
	return pInstance;
}

shared_ptr<Prototype> BackGround::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new BackGround(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : BackGround");
		return nullptr;
	}

	return pInstance;
}