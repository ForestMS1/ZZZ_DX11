#include "pch.h"
#include "CorinWeapon.h"
#include "CorinWeaponScript.h"
CorinWeapon::CorinWeapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: GameObject(pDevice, pDeviceContext)
{
}

CorinWeapon::CorinWeapon(const CorinWeapon& Prototype)
	: GameObject(Prototype)
{
}

CorinWeapon::~CorinWeapon()
{
}

HRESULT CorinWeapon::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CorinWeapon::Initialize(void* pArg)
{
	// 모든 게임 오브젝트는 Create 함수를 통해 생성한다. (자동 Init -> 트랜스폼 추가)
	GameObject::Initialize(pArg);
	GameObject::SetName(L"CorinWeapon");
	GameObject::Set_ClassName(L"CorinWeapon");
	AddComponent(make_shared<CorinWeaponScript>());


	shared_ptr<OBBCollider> collider = make_shared<OBBCollider>();
	AddComponent(collider);

	GetTransform()->SetPosition(Vec3(0, 0, 0));
	GetTransform()->SetScale(Vec3(1.f));
	return S_OK;
}

unique_ptr<CorinWeapon> CorinWeapon::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<CorinWeapon>(new CorinWeapon(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CorinWeapon");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> CorinWeapon::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new CorinWeapon(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CorinWeapon");
		return nullptr;
	}

	return pInstance;
}