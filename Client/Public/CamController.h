#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Client)

class CamController final : public MonoBehaviour
{
public:
	CamController();
	~CamController();

public:
	virtual void Update() override;
	virtual void LateUpdate() override;
};

NS_END