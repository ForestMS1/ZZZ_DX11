#include "pch.h"
#include "Level_Game.h"
#include "Level_Logo.h"

Client::Level_Game::Level_Game(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	:Level(pDevice, pDeviceContext)
{
}

Client::Level_Game::~Level_Game()
{
}

HRESULT Client::Level_Game::Initialize()
{
	return S_OK;
}

void Client::Level_Game::Update()
{

}

HRESULT Client::Level_Game::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, L"게임플레이 레벨임.");
#endif
	return S_OK;
}

unique_ptr<Level_Game> Client::Level_Game::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<Level_Game>(new Level_Game(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(g_hWnd, TEXT("Failed to Created : CLevel_GamePlay"), nullptr, MB_OK);
		return nullptr;
	}
	return pInstance;
}