#include "pch.h"
#include "BackGround.h"
#include "BackGroundTestScript.h"
#include "Transform.h"

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
	return S_OK;
}

HRESULT BackGround::Render()
{
	ImGui::Begin("BackGround Transform");
	Vec3 pos = GetTransform()->GetPosition();
	ImGui::DragFloat3("Transform", (float*)&pos, 1.0f, 0.f, 0.f, "%.3f", 0);
	ImGui::End();
	return S_OK;
}

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