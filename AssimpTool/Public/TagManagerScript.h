#pragma once
#include "MonoBehaviour.h"

class StateMachine;

class TagManagerScript : public MonoBehaviour
{
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnDestroy() override;

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) override { return make_shared<TagManagerScript>(*this); }

	virtual void OnInspectorGUI() override;

public:
	shared_ptr<GameObject> GetCurCharacter();

public:
	static unique_ptr<TagManagerScript> Create();

private:
	void ChangeCharacter(); // 현재 캐릭터 빼고 다음 캐릭터 진입
	void OnQuestStart();

private:
	shared_ptr<GameObject> _playerHpUIBg;

	vector<shared_ptr<StateMachine>> _playerCharacters;
	uint8 _curActivePlayerIndex = 0;
	Vec3 _lastPosition;
	Vec3 _lastRotation;
};

