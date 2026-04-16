#include "pch.h"
#include "AABBCollider.h"
#include "GameObject.h"
AABBCollider::AABBCollider()
	: Collider(ColliderType::AABB)
{
}

AABBCollider::~AABBCollider()
{
}

void AABBCollider::LateUpdate()
{
	Vec3 gameObjectPosition = GetGameObject()->GetTransform()->GetLocalPosition();

	_colliderBox.Center = gameObjectPosition + _offset;
}

void AABBCollider::FixedUpdate()
{
}


bool AABBCollider::Intersects(Ray& ray, OUT float& distance)
{
	return false;
}

bool AABBCollider::Intersects(const shared_ptr<Collider> other)
{
	ColliderType otherType = other->GetColliderType();

	switch (otherType)
	{
	case ColliderType::AABB:
		return _colliderBox.Intersects(static_pointer_cast<AABBCollider>(other)->GetBoundingBox());
		break;
	case ColliderType::OBB:
		break;
	case ColliderType::Sphere:
		break;
	default:
		break;
	}

	return false;
}
