#include "pch.h"
#include "Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"
Level_Manager::Level_Manager()
{
}

HRESULT Level_Manager::Change_Level(uint32 iNewLevelIndex, unique_ptr<Level> pNewLevel)
{
	if (pNewLevel == nullptr)
		return E_FAIL;

	if (_currentLevel != nullptr)
	{
		GameInstance::Get().Clear_Resource(_currentLevelIndex);
		_currentLevel.reset();
	}

	_currentLevelIndex = iNewLevelIndex;

	_currentLevel = std::move(pNewLevel);

	//Level_Manager::Awake();
	//Level_Manager::Start();

	return S_OK;
}

void Level_Manager::Awake()
{
	if (_currentLevel == nullptr)
		return;

	_currentLevel->Awake();
}

void Level_Manager::Start()
{
	if (_currentLevel == nullptr)
		return;

	_currentLevel->Start();
}

void Level_Manager::Update()
{
	if (_currentLevel == nullptr)
		return;

	_currentLevel->Update();
}

void Level_Manager::LateUpdate()
{
	if (_currentLevel == nullptr)
		return;

	_currentLevel->LateUpdate();
}

void Level_Manager::FixedUpdate()
{
	if (_currentLevel == nullptr)
		return;

	_currentLevel->FixedUpdate();
}

HRESULT Level_Manager::Render()
{
	if (_currentLevel == nullptr)
		return E_FAIL;

	return _currentLevel->Render();
}

unique_ptr<Level_Manager> Level_Manager::Create()
{
	return unique_ptr<Level_Manager>(new Level_Manager);
}
