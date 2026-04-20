#include "pch.h"
#include "AABBCollider.h"
#include "GameObject.h"
#include "OBBCollider.h"
#include "SphereCollider.h"
AABBCollider::AABBCollider()
	: Collider(ColliderType::AABB)
{
}

AABBCollider::~AABBCollider()
{
}

void AABBCollider::LateUpdate()
{
	// TODO : 임시 / 디버그 렌더용
	GAME.Add_RenderObject(RENDERGROUP::BLEND, GetGameObject());

	Vec3 gameObjectPosition = GetGameObject()->GetTransform()->GetLocalPosition();

	_colliderBox.Center = gameObjectPosition + _offset;
}

void AABBCollider::FixedUpdate()
{
	
}

HRESULT AABBCollider::Render()
{
	GAME.DrawBox(_colliderBox, _debugColor, Camera::S_MatView, Camera::S_MatProjection);

	return S_OK;
}


bool AABBCollider::Intersects(Ray& ray, OUT float& distance)
{
	return _colliderBox.Intersects(ray.position, ray.direction, OUT distance);
}

bool AABBCollider::Intersects(const shared_ptr<Collider> other)
{
	ColliderType otherType = other->GetColliderType();

	switch (otherType)
	{
	case ColliderType::AABB:
		return _colliderBox.Intersects(dynamic_pointer_cast<AABBCollider>(other)->GetBoundingBox());
		break;
	case ColliderType::OBB:
		return _colliderBox.Intersects(dynamic_pointer_cast<OBBCollider>(other)->GetBoundingOrientedBox());
		break;
	case ColliderType::Sphere:
		return _colliderBox.Intersects(dynamic_pointer_cast<SphereCollider>(other)->GetBoundingSphere());
		break;
	default:
		break;
	}

	return false;
}

void AABBCollider::OnInspectorGUI()
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