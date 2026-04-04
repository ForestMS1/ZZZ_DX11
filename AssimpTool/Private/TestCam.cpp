#include "pch.h"
#include "TestCam.h"
#include "TestCamScript.h"

TestCam::TestCam(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

TestCam::TestCam(const TestCam& Prototype)
	: GameObject(Prototype)
{
}

TestCam::~TestCam()
{
}

HRESULT TestCam::Initialize_Prototype()
{
	return S_OK;
}

HRESULT TestCam::Initialize(void* pArg)
{
	GameObject::Initialize(pArg);
	GameObject::SetName(L"TestCam");
	AddComponent(make_shared<Camera>());
	AddComponent(make_shared<TestCamScript>());

	GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
	return S_OK;
}

HRESULT TestCam::Render()
{

	return S_OK;
}

unique_ptr<TestCam> TestCam::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<TestCam>(new TestCam(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("TestCam Create Failed");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> TestCam::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new TestCam(*this));
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("TestCam Clone Failed");
		return nullptr;
	}

	return pInstance;
}
