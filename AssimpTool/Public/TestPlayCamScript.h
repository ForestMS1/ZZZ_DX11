#pragma once
#include "MonoBehaviour.h"
class TestPlayCamScript : public MonoBehaviour
{
	virtual void Awake() override;
	virtual void LateUpdate() override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return make_shared<TestPlayCamScript>(*this); }

	virtual void OnInspectorGUI() override;


	// 이벤트 함수
private:
	void OnQuestStart();
	void OnCharacterSwitch(const EventDesc& desc);

private:
	void MouseFix();

public:
	static unique_ptr<TestPlayCamScript> Create();

private:
	weak_ptr<GameObject> _target;
	float _pitch = 0.f;	// 상하 회전
	float _yaw = 0.f; // 좌우 회전
	float _roll = 0.f;
private:
	float _curDistance = 5.0f; // 현재 적용할 카메라 거리
private:
	// ... 기존 변수들 (_yaw, _pitch, _isPrevActionPlay 등)
	Vec3 _curLookTarget = Vec3(0.f, 0.f, 0.f); // 현재 카메라가 바라보고 있는 실제 월드 좌표

	bool _mouseFix = true;

private:
	bool _isPrevActionPlay = false;
};