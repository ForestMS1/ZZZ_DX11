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
	weak_ptr<class Collider> colliderA;
	weak_ptr<class Collider> colliderB;
	
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

	// 충돌 감지
	virtual bool Intersects(Ray& ray, OUT float& distance) = 0;
	virtual bool Intersects(const shared_ptr<Collider> other) = 0;


	// Save & Load, 콜라이더마다 Bounding 구조체가 다르기 때문에 가상화
	virtual void SetScale(Vec3 scale) = 0;
	virtual Vec3 GetScale() = 0;

	// 오브젝트의 Transform 중심점으로부터 얼마나 떨어진 위치에 _colliderBox의 중심점을 둘 것인가.
	void SetOffset(Vec3 offset) { _offset = offset; };
	Vec3 GetOffset() { return _offset; }

	bool IsTrigger() const { return _isTrigger; }
	void SetTrigger(bool trigger = true) { _isTrigger = trigger; }

	bool IsFix() const { return _isFix; }
	void SetFix(bool fix = true) { _isFix = fix; }

	// 콜라이더 활성화상태 on/off off면 충돌처리에서 제외
	bool IsActive() const { return _isActive; }
	void SetActive(bool isActive) { _isActive = isActive; }

	ColliderType GetColliderType() const { return _colliderType; }

	// 디버그 컬러 변경
	void SetDebugColor(XMVECTORF32 debugColor) { _debugColor = debugColor; }

	virtual void OnInspectorGUI() override;

protected:
	ColliderType _colliderType = ColliderType::END;
	Vec3 _offset;
	XMVECTORF32 _debugColor = Colors::Lime;

	// 충돌감지만(true일때)
	bool _isTrigger = false;
	// 충돌한만큼 밀려날것인지 (벽, 맵, 몬스터 같은 애들 true)
	bool _isFix = true;


	// 콜라이더 on/off
	bool _isActive = true;
};

NS_END