#pragma once
#include "MonoBehaviour.h"
class TestCorinScript : public MonoBehaviour
{
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnCollisionEnter(const Collision& collision) override;
	virtual void OnCollisionExit(const Collision& collision) override;

private:
	uint32 _collisionCount = 0;
};

