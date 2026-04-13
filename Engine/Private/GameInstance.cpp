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
#include "Resource_Manager.h"
#include "Input_Manager.h"
#include "Shader.h"
#include "GameObjectFactory.h"
#include "imnodes.h"

GameInstance::GameInstance()
{
}

GameInstance::~GameInstance()
{
}

HRESULT GameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext)
{
	::memcpy(&_desc, &EngineDesc, sizeof(ENGINE_DESC));
	_graphicDevice = Graphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, pOutDevice, pOutDeviceContext);
	if (nullptr == _graphicDevice)
		return E_FAIL;

	_timerManager = Timer_Manager::Create();
	if (nullptr == _timerManager)
		return E_FAIL;

	_resourceManager = Resource_Manager::Create();
	if (nullptr == _resourceManager)
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

	_inputManager = Input_Manager::Create(EngineDesc.hInst, EngineDesc.hWnd);
	if (nullptr == _inputManager)
		return E_FAIL;

	_gameObjectFactory = GameObjectFactory::Create();
	if (nullptr == _gameObjectFactory)
		return E_FAIL;

	return S_OK;
}

void GameInstance::Update_Engine()
{
	_inputManager->Update_InputDev();

	_objectManager->BeginFrame();

	_objectManager->Update();

	_objectManager->LateUpdate();

	_objectManager->FixedUpdate();
	
	_objectManager->EndOfFrame();

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

HRESULT GameInstance::Change_Level(uint32 iNewLevelIndex, unique_ptr<Level> pNewLevel, bool clearResourceManager)
{
	if (clearResourceManager) ClearResourceManager();
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
ComPtr<IDXGISwapChain> GameInstance::GetSwapChain()
{ 
	return _graphicDevice->GetSwapChain();
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

shared_ptr<GameObject> GameInstance::Find_GameObject_fromLayer(const wstring& strLayerTag, const wstring& objName)
{
	return _objectManager->Find_GameObject_fromLayer(strLayerTag, objName);
}

void GameInstance::DisableCameras()
{
	_objectManager->DisableCameras();
}

void GameInstance::firstFindCamOn()
{
	_objectManager->firstFindCamOn();
}

void GameInstance::ShowHiearchy()
{
	_objectManager->ShowHiearchy(_desc.ppLevelNames);
}

void GameInstance::ShowInspector()
{
	_objectManager->ShowInspector();
}

void GameInstance::RenderGizmo()
{
	_objectManager->RenderGizmo();
}

const list<shared_ptr<GameObject>>& GameInstance::Get_GameObjects(uint32 iLayerLevelIndex, const wstring& strLayerTag)
{
	return _objectManager->Get_GameObjects(iLayerLevelIndex, strLayerTag);
}
// 프로토타입->클론 없이 바로 레벨에 오브젝트 추가하는 함수
HRESULT GameInstance::Add_GameObject_toLayerNoClone(uint32 iLayerLevelIndex, const wstring& strLayerTag, shared_ptr<GameObject> pGameObject)
{
	return _objectManager->Add_GameObject_toLayerNoClone(iLayerLevelIndex, strLayerTag, pGameObject);
}

HRESULT GameInstance::Add_Layer(uint32 iLayerLevelIndex, const wstring& strLayerTag)
{
	return _objectManager->Add_Layer(iLayerLevelIndex, strLayerTag);
}
// Gui 말고 코드레벨에서 Save-Load 가능하도록 인터페이스 열어줌
void GameInstance::SaveLevel(uint32 iLayerLevelIndex, const wstring& strLayerTag)
{
	_objectManager->SaveLevel(iLayerLevelIndex, strLayerTag);
}
void GameInstance::LoadLevel(uint32 iLayerLevelIndex, const wstring& strLayerTag)
{
	_objectManager->LoadLevel(iLayerLevelIndex, strLayerTag);
}
#pragma endregion

#pragma region RESOURCE_MANAGER
shared_ptr<Texture> GameInstance::GetOrAddTexture(const wstring& key, const wstring& path)
{
	return _resourceManager->GetOrAddTexture(key, path);
}

void GameInstance::ClearResourceManager()
{
	_resourceManager->ClearResource();
}

void GameInstance::ShowResourceList()
{
	_resourceManager->ShowResourceList();
}
#pragma endregion
#pragma region RENDERER

HRESULT GameInstance::Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<GameObject> pRenderObject)
{
	return _renderer->Add_RenderObject(eRenderGroup, pRenderObject);
}
#pragma endregion

#pragma region INPUT_MANAGER
signed char	GameInstance::Get_DIKeyState(unsigned char byKeyID)
{
	return _inputManager->Get_DIKeyState(byKeyID);
}

signed char	GameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return _inputManager->Get_DIMouseState(eMouse);
}

// 현재 마우스의 특정 축 좌표를 반환
signed long	GameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return _inputManager->Get_DIMouseMove(eMouseState);
}

bool GameInstance::Key_Pressing(unsigned char byKeyID)
{
	return _inputManager->Key_Pressing(byKeyID);
}
bool GameInstance::Key_Up(unsigned char byKeyID)
{
	return _inputManager->Key_Up(byKeyID);
}
bool GameInstance::Key_Down(unsigned char byKeyID)
{
	return _inputManager->Key_Down(byKeyID);
}

bool GameInstance::Mouse_Pressing(MOUSEKEYSTATE eMouseState)
{
	return _inputManager->Mouse_Pressing(eMouseState);
}
bool GameInstance::Mouse_Up(MOUSEKEYSTATE eMouseState)
{
	return _inputManager->Mouse_Up(eMouseState);
}
bool GameInstance::Mouse_Down(MOUSEKEYSTATE eMouseState)
{
	return _inputManager->Mouse_Down(eMouseState);
}
#pragma endregion

#pragma region GAMEOBJECTFACTORY
void GameInstance::Register(const wstring& className, CreatorFunc func)
{
	_gameObjectFactory->Register(className, func);
}

shared_ptr<GameObject> GameInstance::CreateFromFactory(const wstring& className)
{
	return _gameObjectFactory->CreateFromFactory(className);
}
#pragma endregion

void GameInstance::SetEngineContext(ImGuiContext* pContext, ImNodesContext* pNodesContext)
{
	// DLL 영역의 전역 변수 GImGui를 EXE에서 만든 컨텍스트로 셋팅
	ImGui::SetCurrentContext(pContext);
	ImNodes::SetCurrentContext(pNodesContext);
}

void GameInstance::Release_Engine()
{
	ShaderManager::Clear();
	_inputManager.reset();
	_renderer.reset();
	_levelManager.reset();
	_timerManager.reset();
	_objectManager.reset();
	_prototypeManager.reset();
	_resourceManager.reset();
	_gameObjectFactory.reset();
	_graphicDevice->ShutDown();
	_graphicDevice.reset();
}