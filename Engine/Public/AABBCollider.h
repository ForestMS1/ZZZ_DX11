#pragma once
#include "Collider.h"

NS_BEGIN(Engine)

class ENGINE_DLL AABBCollider :  public Collider
{
public:
	AABBCollider();
	~AABBCollider();

public:

	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

	bool Intersects(Ray& ray, OUT float& distance) override;
	bool Intersects(const shared_ptr<Collider> other) override;


	// 플레이어 Transform 중심점으로부터 얼마나 떨어진 위치에 _colliderBox의 중심점을 둘 것인가.
	void SetOffset(Vec3 offset) { _offset = offset; };
	// _colliderBox의 스케일.
	void SetScale(Vec3 scale) { _colliderBox.Extents = scale; };

	BoundingBox& GetBoundingBox() { return _colliderBox; }

	shared_ptr<Prototype> Clone(void* pArg) override { return nullptr; };

private:
	BoundingBox _colliderBox;
	Vec3 _offset;
};

NS_END
