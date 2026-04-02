#pragma once
#include "MonoBehaviour.h"

NS_BEGIN(Client)

class CubeScritp : public MonoBehaviour
{
public:
	CubeScritp();
	virtual ~CubeScritp();

	virtual void Update() override;

};

NS_END
