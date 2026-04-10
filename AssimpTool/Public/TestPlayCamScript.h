#pragma once
#include "MonoBehaviour.h"
class TestPlayCamScript : public MonoBehaviour
{
	virtual void Awake() override;
	virtual void LateUpdate() override;

private:
	shared_ptr<GameObject> _target;
	float _pitch = 0.f;	// 상하 회전
	float _yaw = 0.f; // 좌우 회전
	float _roll = 0.f;

	bool _mouseFix = true;
};