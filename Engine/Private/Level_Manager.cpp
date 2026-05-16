#include "pch.h"
#include "Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"
Level_Manager::Level_Manager()
{
}

HRESULT Level_Manager::Change_Level(uint32_t iNewLevelIndex, unique_ptr<Level> pNewLevel, bool clearResource)
{
	if (nullptr == pNewLevel)
		return E_FAIL;

	if (nullptr != _currentLevel)
	{
		GAME.Clear_Resource(_currentLevelIndex);
		if (clearResource) GAME.ClearResourceManager();
		_currentLevel.reset();
	}

	_currentLevel = std::move(pNewLevel);

	_currentLevelIndex = iNewLevelIndex;

	return S_OK;
}

void Level_Manager::Awake()
{
	if (_currentLevel == nullptr)
		return;

	if (_currentLevel->GetLifeState() == LIFESTATE::NONE)
	{
		_currentLevel->Awake();
		_currentLevel->SetLifeState(LIFESTATE::AWAKED);
	}
}

void Level_Manager::Start()
{
	if (_currentLevel == nullptr)
		return;

	if (_currentLevel->GetLifeState() == LIFESTATE::AWAKED)
	{
		_currentLevel->Start();
		_currentLevel->SetLifeState(LIFESTATE::STARTED);
	}
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
