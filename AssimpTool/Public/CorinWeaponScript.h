#pragma once
#include "MonoBehaviour.h"

class CorinWeaponScript : public MonoBehaviour
{
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

	virtual void OnCollisionEnter(const Collision& collision) override;
	virtual void OnCollisionStay(const Collision& collision) override;
	virtual void OnCollisionExit(const Collision& collision) override;

	virtual void OnDestroy() override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return make_shared<CorinWeaponScript>(*this); }

	virtual void OnInspectorGUI() override;

public:
	static unique_ptr<CorinWeaponScript> Create();

private:
	uint32 _collisionCount = 0;

	shared_ptr<Transform> _transformCom;
	shared_ptr<ModelAnimator> _modelAnimator;
	shared_ptr<Model> _model;

	Vec3 _offset = {0.6f, 0.9f, 0.f};
};

