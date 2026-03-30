#include "pch.h"
#include "Component.h"
#include "GameObject.h"

Component::Component(ComponentType eType) : _type(eType)
{
}

Component::~Component()
{
}

shared_ptr<GameObject> Component::GetGameObject()
{
	return _gameObject.lock();
}

shared_ptr<Transform> Component::GetTransform()
{
	return _gameObject.lock()->GetTransform();
}


shared_ptr<Prototype> Component::Clone(void* pArg)
{
	return shared_ptr<Prototype>();
}
