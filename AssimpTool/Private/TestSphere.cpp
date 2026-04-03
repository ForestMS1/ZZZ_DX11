#include "pch.h"
#include "TestSphere.h"
#include "Transform.h"
#include "Model.h"
#include "ModelRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "TestSphereScript.h"
TestSphere::TestSphere(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

TestSphere::TestSphere(const TestSphere& Prototype)
	: GameObject(Prototype)
{
}

TestSphere::~TestSphere()
{
}

HRESULT TestSphere::Initialize_Prototype()
{

	return S_OK;
}

HRESULT TestSphere::Initialize(void* pArg)
{
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	GameObject::Initialize(pArg);
	AddComponent(make_shared<TestSphereScript>());

	return S_OK;
}

unique_ptr<TestSphere> TestSphere::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<TestSphere>(new TestSphere(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TestSphere");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> TestSphere::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new TestSphere(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TestSphere");
		return nullptr;
	}

	return pInstance;
}