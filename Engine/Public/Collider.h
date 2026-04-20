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
	
	// x,y,z 축 얼마나 겹치는지
	float overlapX;
	float overlapY;
	float overlapZ;
};

class ENGINE_DLL Collider abstract : public Component
{
public:
	Collider(ColliderType type);
	virtual ~Collider();

public:

	virtual bool Intersects(Ray& ray, OUT float& distance) = 0;
	virtual bool Intersects(const shared_ptr<Collider> other) = 0;

	virtual void SetScale(Vec3 scale) = 0;
	virtual Vec3 GetScale() = 0;


	// 플레이어 Transform 중심점으로부터 얼마나 떨어진 위치에 _colliderBox의 중심점을 둘 것인가.
	void SetOffset(Vec3 offset) { _offset = offset; };
	Vec3 GetOffset() { return _offset; }

	ColliderType GetColliderType() const { return _colliderType; }


	bool IsTrigger() const { return _isTrigger; }
	void SetTrigger(bool trigger = true) { _isTrigger = trigger; }

	// 디버그 컬러 변경
	void SetDebugColor(XMVECTORF32 debugColor) { _debugColor = debugColor; }

	virtual void OnInspectorGUI() override;

protected:
	ColliderType _colliderType = ColliderType::END;
	Vec3 _offset;
	XMVECTORF32 _debugColor = Colors::Lime;

	bool _isTrigger = false;
};

NS_END