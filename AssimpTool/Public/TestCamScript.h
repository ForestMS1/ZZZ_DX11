#pragma once
#include "MonoBehaviour.h"
class TestCamScript : public MonoBehaviour
{
	virtual void LateUpdate() override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return make_shared<TestCamScript>(); }

	virtual void OnInspectorGUI() override;

public:
	static unique_ptr<TestCamScript> Create();
};

