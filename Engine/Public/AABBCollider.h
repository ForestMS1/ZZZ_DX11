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

	virtual HRESULT Render() override;

	bool Intersects(Ray& ray, OUT float& distance) override;
	bool Intersects(const shared_ptr<Collider> other) override;

	// _colliderBox의 스케일.
	virtual void SetScale(Vec3 scale) override { _colliderBox.Extents = scale; };
	virtual Vec3 GetScale() override { return _colliderBox.Extents; }

	BoundingBox& GetBoundingBox() { return _colliderBox; }

	virtual void OnInspectorGUI() override;

	shared_ptr<Prototype> Clone(void* pArg) override { return nullptr; };

private:
	BoundingBox _colliderBox;
};

NS_END
