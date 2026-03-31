#include "pch.h"
#include "Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"

Level_Logo::Level_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: Level(pDevice, pDeviceContext)
{
}

Level_Logo::~Level_Logo()
{
}

HRESULT Level_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	return S_OK;
}

void Level_Logo::Update()
{
	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		if(FAILED(GameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING),
			Level_Loading::Create(_device, _deviceContext, LEVEL::GAMEPLAY))))
			return;
	}
}

HRESULT Level_Logo::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, L"로고 레벨임.");
	ImGui::Begin("Logo Level");
	ImGui::End();
#endif
	return S_OK;
}

HRESULT Level_Logo::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if(FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::LOGO), L"Prototype_GameObject_BackGround",
		ETOUI(LEVEL::LOGO), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

unique_ptr<Level_Logo> Level_Logo::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<Level_Logo>(new Level_Logo(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(g_hWnd, TEXT("Failed to Created : Level_Logo"), nullptr, MB_OK);
		return nullptr;
	}

	return pInstance;
}
