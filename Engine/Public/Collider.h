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

struct Collision
{
	shared_ptr<class Collider> colliderA;
	shared_ptr<class Collider> colliderB;
};

class ENGINE_DLL Collider abstract : public Component
{
public:
	Collider(ColliderType type);
	virtual ~Collider();

public:

	virtual bool Intersects(Ray& ray, OUT float& distance) = 0;
	virtual bool Intersects(const shared_ptr<Collider> other) = 0;

	// 플레이어 Transform 중심점으로부터 얼마나 떨어진 위치에 _colliderBox의 중심점을 둘 것인가.
	void SetOffset(Vec3 offset) { _offset = offset; };

	ColliderType GetColliderType() const { return _colliderType; }

	// 디버그 컬러 변경
	void SetDebugColor(XMVECTORF32 debugColor) { _debugColor = debugColor; }

	virtual void OnInspectorGUI() override;

protected:
	ColliderType _colliderType = ColliderType::END;
	Vec3 _offset;
	XMVECTORF32 _debugColor = Colors::Lime;


	// 지금 하나라도 충돌중인가
	bool _isCollision = false; 
};

NS_END