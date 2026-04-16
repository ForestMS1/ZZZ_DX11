#include "pch.h"
#include "Collider.h"

Collider::Collider(ColliderType type)
	: Component(ComponentType::Collider), _colliderType(type)
{
}

Collider::~Collider()
{
}

