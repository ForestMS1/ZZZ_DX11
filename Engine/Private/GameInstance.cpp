#include "pch.h"
#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Prototype_Manager.h"
#include "Prototype.h"
#include "GameObject.h"
#include "Level.h"
#include "Object_Manager.h"
#include "Renderer.h"

GameInstance::GameInstance()
{
}

GameInstance::~GameInstance()
{
}

HRESULT GameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext)
{
	_graphicDevice = Graphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, pOutDevice, pOutDeviceContext);
	if (nullptr == _graphicDevice)
		return E_FAIL;

	_timerManager = Timer_Manager::Create();
	if (nullptr == _timerManager)
		return E_FAIL;

	_prototypeManager = Prototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == _prototypeManager)
		return E_FAIL;

	_objectManager = Object_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == _objectManager)
		return E_FAIL;

	_renderer = Renderer::Create(pOutDevice, pOutDeviceContext);
	if (nullptr == _renderer)
		return E_FAIL;

	_levelManager = Level_Manager::Create();
	if (nullptr == _levelManager)
		return E_FAIL;

	return S_OK;
}

void GameInstance::Update_Engine()
{
	_objectManager->BeginFrame();

	_objectManager->Update();

	_objectManager->LateUpdate();

	_objectManager->FixedUpdate();

	_levelManager->Update();
}

HRESULT GameInstance::Draw()
{
	if (FAILED(_renderer->Draw()))
		return E_FAIL;

	if (FAILED(_levelManager->Render()))
		return E_FAIL;

	return S_OK;
}

void GameInstance::Clear_Resource(uint32 iClearLevelIndex)
{
	_objectManager->Clear(iClearLevelIndex);

	_prototypeManager->Clear(iClearLevelIndex);
}

#pragma region TIMER_MANAGER

float GameInstance::Get_TimeDelta(const wstring& strTimerTag)
{
	return _timerManager->Get_TimeDelta(strTimerTag);
}

void GameInstance::Compute_TimeDelta(const wstring& strTimerTag)
{
	_timerManager->Set_TimeDelta(strTimerTag);
}

HRESULT GameInstance::Add_Timer(const wstring& strTimerTag)
{
	return _timerManager->Ready_Timer(strTimerTag);
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT GameInstance::Change_Level(uint32 iNewLevelIndex, unique_ptr<Level> pNewLevel)
{
	return _levelManager->Change_Level(iNewLevelIndex, std::move(pNewLevel));
}

uint32 GameInstance::GetCurrentLevelIndex()
{
	return _levelManager->GetCurrentLevelIndex();
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

ComPtr<ID3D11Device> GameInstance::GetDevice()
{
	return _graphicDevice->GetDevice();
}
ComPtr<ID3D11DeviceContext> GameInstance::GetDeviceContext()
{
	return _graphicDevice->GetDeviceContext();
}

#pragma endregion

#pragma region PROTOTYPE_MANAGER
HRESULT GameInstance::Add_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag, unique_ptr<Prototype> pPrototype)
{
	return _prototypeManager->Add_Prototype(iLevelIndex, strPrototypeTag, std::move(pPrototype));
}

shared_ptr<Prototype> GameInstance::Clone_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	return _prototypeManager->Clone_Prototype(iLevelIndex, strPrototypeTag, pArg);
}
#pragma endregion

#pragma region OBJECT_MANAGER
HRESULT GameInstance::Add_GameObject_toLayer(uint32 iPrototypeLevelIndex, const wstring& strPrototypeTag,
	uint32 iLayerLevelIndex, const wstring& strLayerTag, void* pArg)
{
 	return _objectManager->Add_GameObject_toLayer(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}
#pragma endregion

#pragma region RENDERER

HRESULT GameInstance::Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<GameObject> pRenderObject)
{
	return _renderer->Add_RenderObject(eRenderGroup, pRenderObject);
}
#pragma endregion


void GameInstance::Release_Engine()
{
	_renderer.reset();
	_levelManager.reset();
	_timerManager.reset();
	_objectManager.reset();
	_prototypeManager.reset();
	_graphicDevice->ShutDown();
	_graphicDevice.reset();
}