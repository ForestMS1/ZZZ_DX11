#include "pch.h"
#include "MainCam.h"
#include "Camera.h"
#include "CamController.h"
#include "Transform.h"

MainCam::MainCam(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

MainCam::MainCam(const MainCam& Prototype)
	: GameObject(Prototype)
{
}

MainCam::~MainCam()
{
}

HRESULT MainCam::Initialize_Prototype()
{
	return S_OK;
}

HRESULT MainCam::Initialize(void* pArg)
{
	GameObject::Initialize(pArg);
	AddComponent(make_shared<Camera>());
	AddComponent(make_shared<CamController>());

	GetTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
	return S_OK;
}

HRESULT MainCam::Render()
{

	return S_OK;
}

unique_ptr<MainCam> MainCam::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<MainCam>(new MainCam(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("MainCam Create Failed");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> MainCam::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new MainCam(*this));
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("MainCam Clone Failed");
		return nullptr;
	}

	return pInstance;
}
