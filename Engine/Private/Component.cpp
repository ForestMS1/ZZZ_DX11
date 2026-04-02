#include "pch.h"
#include "Component.h"
#include "GameObject.h"

Component::Component(ComponentType eType) : _type(eType)
{
}

Component::~Component()
{
}

HRESULT Component::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Component::Initialize(void* pArg)
{
	return S_OK;
}

shared_ptr<GameObject> Component::GetGameObject()
{
	return _gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform()
{
	return _gameObject.lock()->GetTransform();
}