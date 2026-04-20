#pragma once
#include "Collider.h"

NS_BEGIN(Engine)

class ENGINE_DLL SphereCollider : public Collider
{
public:
	SphereCollider();
	~SphereCollider();

public:
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

	virtual HRESULT Render() override;

	bool Intersects(Ray& ray, OUT float& distance) override;
	bool Intersects(const shared_ptr<Collider> other) override;


	// _colliderBox의 스케일.
	virtual void SetScale(Vec3 scale) override { _colliderSphere.Radius = scale.x; };
	virtual Vec3 GetScale() override { return Vec3(_colliderSphere.Radius); }

	BoundingSphere& GetBoundingSphere() { return _colliderSphere; }

	virtual void OnInspectorGUI() override;

	shared_ptr<Prototype> Clone(void* pArg) override { return nullptr; };

private:
	BoundingSphere _colliderSphere;
};

NS_END
