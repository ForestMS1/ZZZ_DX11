#pragma once
#include "Component.h"

NS_BEGIN(Engine)

enum class ColliderType
{
	AABB,
	OBB,
	Sphere,
	END
};

class ENGINE_DLL Collider abstract : public Component
{
public:
	Collider(ColliderType type);
	virtual ~Collider();

public:

	// TODO : 이벤트 함수 만들기
	// OnCollisionEnter
	// OnCollisionStay
	// OnCollisionExit 

	virtual bool Intersects(Ray& ray, OUT float& distance) = 0;
	virtual bool Intersects(const shared_ptr<Collider> other) = 0;

	ColliderType GetColliderType() const { return _colliderType; }

private:
	ColliderType _colliderType = ColliderType::END;

};

NS_END