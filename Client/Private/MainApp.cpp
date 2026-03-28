#include "pch.h"
#include "MainApp.h"
#include "GameInstance.h"


Client::MainApp::MainApp()
{
}

Client::MainApp::~MainApp()
{
}

HRESULT Client::MainApp::Initialize()
{
	ENGINE_DESC EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ETOUI(LEVEL::END);

	if (FAILED(GameInstance::Get().Initialize_Engine(EngineDesc, _device, _deviceContext)))
		return E_FAIL;

	//if (FAILED(Start_Level(LEVEL::LOGO)))
	//	return E_FAIL;

	return S_OK;
}

void Client::MainApp::Update()
{

}

HRESULT Client::MainApp::Render()
{
	return S_OK;
}

HRESULT Client::MainApp::Start_Level(LEVEL eStartLevelIndex)
{
	return S_OK;
}

unique_ptr<MainApp> Client::MainApp::Create()
{
	auto pInstance = unique_ptr<MainApp>(new MainApp);
	if (FAILED(pInstance->Initialize()))
		return nullptr;

	return pInstance;
}