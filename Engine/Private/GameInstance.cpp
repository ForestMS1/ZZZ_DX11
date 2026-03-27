#include "pch.h"
#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include"Timer_Manager.h"


GameInstance::GameInstance()
{
}

GameInstance::~GameInstance()
{
}

HRESULT GameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device> pOutDevice, ComPtr<ID3D11DeviceContext> pOutDeviceContext)
{
	_graphicDevice = Graphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, pOutDevice, pOutDeviceContext);
	
	if (nullptr == _graphicDevice)
		return E_FAIL;

	_timerManager = Timer_Manager::Create();
	
	if (nullptr == _timerManager)
		return E_FAIL;

	//_prototypeManager = Prototype_Manager::Create(EngineDesc.iNumLevels);
	//if (nullptr == _prototypeManager)
	//	return E_FAIL;

	_levelManager = Level_Manager::Create();

	if (nullptr == _levelManager)
		return E_FAIL;

}

void GameInstance::Update_Engine()
{
	_levelManager->Update();
}

HRESULT GameInstance::Draw()
{
	if (FAILED(_levelManager->Render()))
		return E_FAIL;

	return S_OK;
}

void GameInstance::Clear_Resource(uint32 iClearLevelIndex)
{

}

#pragma region TIMER_MANAGER

float GameInstance::Get_TimeDelta(const wstring& strTimerTag)
{
	return _timerManager->Get_TimeDelta(strTimerTag);
}

void GameInstance::Set_TimeDelta(const wstring& strTimerTag)
{
	_timerManager->Set_TimeDelta(strTimerTag);
}

HRESULT GameInstance::Ready_Timer(const wstring& strTimerTag)
{
	return _timerManager->Ready_Timer(strTimerTag);
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT GameInstance::Chanage_Level(uint32 iNewLevelIndex, unique_ptr<class Level> pNewLevel)
{
	return _levelManager->Change_Level(iNewLevelIndex, std::move(pNewLevel));
}

#pragma endregion

#pragma region GRAPHIC_DEVICE

HRESULT GameInstance::Clear_BackBuffer_View(const Vec4* pClearColor)
{
	return _graphicDevice->Clear_BackBuffer_View(pClearColor);
}

HRESULT GameInstance::Clear_DepthStencil_View()
{
	return _graphicDevice->Clear_DepthStencil_View();
}

HRESULT GameInstance::Present()
{
	return _graphicDevice->Present();
}

#pragma endregion

void GameInstance::Release_Engine()
{
	_levelManager.reset();
	_timerManager.reset();
	//_prototypeManager.reset();
	_graphicDevice->ShutDown();
	_graphicDevice.reset();
}