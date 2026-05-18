#pragma once
#include "MonoBehaviour.h"

class AlpecaInfestedScript : public MonoBehaviour
{
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnCollisionEnter(const Collision& collision) override;
	virtual void OnCollisionStay(const Collision& collision) override;
	virtual void OnCollisionExit(const Collision& collision) override;

	virtual void OnDestroy() override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return make_shared<AlpecaInfestedScript>(*this); }

	virtual void OnInspectorGUI() override;

public:
	static unique_ptr<AlpecaInfestedScript> Create();

private:
	float _onHitDelay = 0.5f;
	float _curHitDelay = 0.f;
};

