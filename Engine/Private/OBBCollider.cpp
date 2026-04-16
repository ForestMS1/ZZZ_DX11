#include "pch.h"
#include "OBBCollider.h"
#include "GameObject.h"
#include "AABBCollider.h"
#include "SphereCollider.h"
OBBCollider::OBBCollider()
	: Collider(ColliderType::OBB)
{
}

OBBCollider::~OBBCollider()
{
}

void OBBCollider::LateUpdate()
{
	Matrix world = GetGameObject()->GetTransform()->GetWorldMatrix();

	Vec3 scale, position;
	Quaternion quat;
	world.Decompose(scale, quat, position);

	_colliderBox.Center = position + _offset;
	_colliderBox.Orientation = quat;
}

void OBBCollider::FixedUpdate()
{

}

HRESULT OBBCollider::Render()
{
	GAME.DrawOrientedBox(_colliderBox, _debugColor, Camera::S_MatView, Camera::S_MatProjection);

	return S_OK;
}

bool OBBCollider::Intersects(Ray& ray, OUT float& distance)
{
	return _colliderBox.Intersects(ray.position, ray.direction, OUT distance);
}

bool OBBCollider::Intersects(const shared_ptr<Collider> other)
{
	ColliderType type = other->GetColliderType();

	switch (type)
	{
	case ColliderType::Sphere:
		return _colliderBox.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());
	case ColliderType::AABB:
		return _colliderBox.Intersects(dynamic_pointer_cast<AABBCollider>(other)->GetBoundingBox());
	case ColliderType::OBB:
		return _colliderBox.Intersects(dynamic_pointer_cast<OBBCollider>(other)->GetBoundingOrientedBox());
	}

	return false;
}

void OBBCollider::OnInspectorGUI()
{
	Collider::OnInspectorGUI();

	if (ImGui::TreeNodeEx("AABB Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Center
		float center[3] = { _colliderBox.Center.x, _colliderBox.Center.y, _colliderBox.Center.z };
		if (ImGui::DragFloat3("Center", center, 0.1f))
		{
			_colliderBox.Center = { center[0], center[1], center[2] };
		}

		// Extents
		float extents[3] = { _colliderBox.Extents.x, _colliderBox.Extents.y, _colliderBox.Extents.z };
		if (ImGui::DragFloat3("Extents", extents, 0.1f))
		{
			_colliderBox.Extents = { max(0.f, extents[0]),
									 max(0.f, extents[1]),
									 max(0.f, extents[2]) };
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
