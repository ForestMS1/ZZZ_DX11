#include "pch.h"
#include "ToolApp.h"
#include "GameInstance.h"

#ifdef _DEBUG
#include "ImGui_Manager.h"
#endif
#include "Level_Loading.h"
AssimpTool::ToolApp::ToolApp()
{
}

AssimpTool::ToolApp::~ToolApp()
{
#ifdef _DEBUG
	ImGui_Manager::Release();
#endif
	GameInstance::Get().Release_Engine();
}

HRESULT AssimpTool::ToolApp::Initialize()
{
	ENGINE_DESC EngineDesc{};
	EngineDesc.hInst = g_hInstance;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ETOUI(LEVEL::END);
	EngineDesc.ppLevelNames = LEVEL_NAMES;

	if (FAILED(GameInstance::Get().Initialize_Engine(EngineDesc, _device, _deviceContext)))
		return E_FAIL;

#ifdef _DEBUG
	ImGui_Manager::Get().Initialize(GameInstance::Get().GetDevice(), GameInstance::Get().GetDeviceContext(), g_hWnd);
#endif

	if (FAILED(Start_Level(LEVEL::TESTMESH)))
		return E_FAIL;

	return S_OK;
}

void AssimpTool::ToolApp::Update()
{

#ifdef _DEBUG
	ImGui_Manager::Get().Update();
#endif
	GameInstance::Get().Update_Engine();

}

HRESULT AssimpTool::ToolApp::Render()
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

HRESULT AssimpTool::ToolApp::Start_Level(LEVEL eStartLevelIndex)
{
	if (FAILED(GameInstance::Get().Change_Level(static_cast<uint32>(LEVEL::LOADING),
		Level_Loading::Create(_device, _deviceContext, eStartLevelIndex))))
		return E_FAIL;
	return S_OK;
}

unique_ptr<ToolApp> AssimpTool::ToolApp::Create()
{
	auto pInstance = unique_ptr<ToolApp>(new ToolApp);
	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(g_hWnd, TEXT("Failed to Created : ToolApp"), nullptr, MB_OK);
		return nullptr;
	}

	return pInstance;
}