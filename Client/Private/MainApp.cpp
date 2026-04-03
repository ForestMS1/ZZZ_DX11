#include "pch.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Game.h"

#ifdef _DEBUG
#include "ImGui_Manager.h"
#endif
Client::MainApp::MainApp()
{
}

Client::MainApp::~MainApp()
{
#ifdef _DEBUG
	ImGui_Manager::Release();
#endif
	GameInstance::Get().Release_Engine();
}

HRESULT Client::MainApp::Initialize()
{
	ENGINE_DESC EngineDesc{};
	EngineDesc.hInst = g_hInstance;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ETOUI(LEVEL::END);

	if (FAILED(GameInstance::Get().Initialize_Engine(EngineDesc, _device, _deviceContext)))
		return E_FAIL;

#ifdef _DEBUG
	ImGui_Manager::Get().Initialize(GameInstance::Get().GetDevice(), GameInstance::Get().GetDeviceContext(), g_hWnd);
#endif

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;

	return S_OK;
}

void Client::MainApp::Update()
{

#ifdef _DEBUG
	ImGui_Manager::Get().Update();
#endif
	GameInstance::Get().Update_Engine();
	
}

HRESULT Client::MainApp::Render()
{
	Vec4 vClearColor = { 0.f, 0.f, 1.f, 1.f };

	if (FAILED(GameInstance::Get().Clear_BackBuffer_View(&vClearColor)))
		return E_FAIL;

	if (FAILED(GameInstance::Get().Clear_DepthStencil_View()))
		return E_FAIL;

	if (FAILED(GameInstance::Get().Draw()))
		return E_FAIL;

#ifdef _DEBUG
	ImGui_Manager::Get().Render();
#endif

	if (FAILED(GameInstance::Get().Present()))
		return E_FAIL;



	return S_OK;
}

HRESULT Client::MainApp::Start_Level(LEVEL eStartLevelIndex)
{
	if (FAILED(GameInstance::Get().Change_Level(static_cast<uint32>(LEVEL::LOADING),
		Level_Loading::Create(_device, _deviceContext, eStartLevelIndex))))
		return E_FAIL;
	return S_OK;
}

unique_ptr<MainApp> Client::MainApp::Create()
{
	auto pInstance = unique_ptr<MainApp>(new MainApp);
	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(g_hWnd, TEXT("Failed to Created : MainApp"), nullptr, MB_OK);
		return nullptr;
	}

	return pInstance;
}