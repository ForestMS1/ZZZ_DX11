#include "pch.h"
#include "TestPlayCam.h"
#include "TestPlayCamScript.h"
TestPlayCam::TestPlayCam(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

TestPlayCam::TestPlayCam(const TestPlayCam& Prototype)
	: GameObject(Prototype)
{
}

TestPlayCam::~TestPlayCam()
{
}

HRESULT TestPlayCam::Initialize_Prototype()
{
	return S_OK;
}

HRESULT TestPlayCam::Initialize(void* pArg)
{
	GameObject::Initialize(pArg);
	GameObject::SetName(L"TestPlayCam");
	GameObject::Set_ClassName(L"TestPlayCam");
	AddComponent(make_shared<Camera>());
	AddComponent(make_shared<TestPlayCamScript>());

	GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
	return S_OK;
}

unique_ptr<TestPlayCam> TestPlayCam::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<TestPlayCam>(new TestPlayCam(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("TestPlayCam Create Failed");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> TestPlayCam::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new TestPlayCam(*this));
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("TestPlayCam Clone Failed");
		return nullptr;
	}

	return pInstance;
}
