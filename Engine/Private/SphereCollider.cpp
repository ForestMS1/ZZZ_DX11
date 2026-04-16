#include "pch.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "AABBCollider.h"
#include "OBBCollider.h"
SphereCollider::SphereCollider()
	: Collider(ColliderType::Sphere)
{
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::LateUpdate()
{
	Vec3 gameObjectPosition = GetGameObject()->GetTransform()->GetLocalPosition();

	_colliderSphere.Center = gameObjectPosition + _offset;
}

void SphereCollider::FixedUpdate()
{
}

HRESULT SphereCollider::Render()
{
	GAME.DrawSphereBox(_colliderSphere, _debugColor, Camera::S_MatView, Camera::S_MatProjection);

	return S_OK;
}

bool SphereCollider::Intersects(Ray& ray, OUT float& distance)
{
	return _colliderSphere.Intersects(ray.position, ray.direction, OUT distance);
}

bool SphereCollider::Intersects(const shared_ptr<Collider> other)
{
	ColliderType type = other->GetColliderType();

	switch (type)
	{
	case ColliderType::Sphere:
		return _colliderSphere.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());
	case ColliderType::AABB:
		return _colliderSphere.Intersects(dynamic_pointer_cast<AABBCollider>(other)->GetBoundingBox());
	case ColliderType::OBB:
		return _colliderSphere.Intersects(dynamic_pointer_cast<OBBCollider>(other)->GetBoundingOrientedBox());
	}

	return false;
}

void SphereCollider::OnInspectorGUI()
{
	Collider::OnInspectorGUI();

	if (ImGui::TreeNodeEx("AABB Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Center
		float center[3] = { _colliderSphere.Center.x, _colliderSphere.Center.y, _colliderSphere.Center.z };
		if (ImGui::DragFloat3("Center", center, 0.1f))
		{
			_colliderSphere.Center = { center[0], center[1], center[2] };
		}

		// Radius
		float radius = _colliderSphere.Radius;
		if (ImGui::DragFloat("Radius", &radius, 0.1f))
		{
			_colliderSphere.Radius = max(0.f, radius);
		}

		// Offset
		float offset[3] = { _offset.x, _offset.y, _offset.z };
		if (ImGui::DragFloat3("Offset", offset, 0.1f))
		{
			_offset = { max(0.f, offset[0]),
						max(0.f, offset[1]),
						max(0.f, offset[2]) };
		}

		ImGui::TreePop();
	}
}
