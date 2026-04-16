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

	// 플레이어 Transform 중심점으로부터 얼마나 떨어진 위치에 _colliderBox의 중심점을 둘 것인가.
	void SetOffset(Vec3 offset) { _offset = offset; };

	ColliderType GetColliderType() const { return _colliderType; }

	virtual void OnInspectorGUI() override;

protected:
	ColliderType _colliderType = ColliderType::END;
	Vec3 _offset;
	XMVECTORF32 _debugColor = Colors::Lime;
};

NS_END