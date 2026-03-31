#pragma once

#include "MonoBehaviour.h"

class BackGroundTestScript final : public MonoBehaviour
{
public:
	BackGroundTestScript();
	~BackGroundTestScript();

	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;
	void FixedUpdate() override;

	// MonoBehaviour을(를) 통해 상속됨
	shared_ptr<Prototype> Clone(void* pArg) override;
};

