#include "pch.h"
#include "Level_Loading.h"

#include "Loader.h"

#include "Level_Logo.h"
#include "Level_Game.h"
#include "GameInstance.h"

Level_Loading::Level_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: Level(pDevice, pDeviceContext)
{
}

Level_Loading::~Level_Loading()
{
}

HRESULT Level_Loading::Initialize(LEVEL eNextLevelIndex)
{
	_nextLevelIndex = eNextLevelIndex;

	/* 로딩에 필요한 객체를 생성한다.  */
	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	/* 다음레벨에 필요한 자원을 로드해주기위한 준비작업(로딩작업을 수행해줄 스레드를 생성한다)을 수행한다. */
	_loader = Loader::Create(_device, _deviceContext, eNextLevelIndex);
	if (nullptr == _loader)
		return E_FAIL;

	return S_OK;
}

void Level_Loading::Update()
{
	if (true == _loader->IsFinished() &&
		GetKeyState(VK_RETURN) & 0x8000)
	{
		unique_ptr<Level> pNewLevel = { nullptr };

		switch (_nextLevelIndex)
		{
		case LEVEL::LOGO:
			pNewLevel = Level_Logo::Create(_device, _deviceContext);
			break;
		case LEVEL::GAMEPLAY:
			pNewLevel = Level_Game::Create(_device, _deviceContext);
			break;
		default:
			break;
		}

		if(FAILED(GameInstance::Get().Change_Level(ETOUI(_nextLevelIndex), std::move(pNewLevel))))
			return;
	}
}

HRESULT Level_Loading::Render()
{
#ifdef _DEBUG
	_loader->Output_LoadingText();
#endif

	return S_OK;
}

HRESULT Level_Loading::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	return S_OK;
}

unique_ptr<Level_Loading> Level_Loading::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext, LEVEL eNextLevelIndex)
{
	auto pInstance = unique_ptr<Level_Loading>(new Level_Loading(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize(eNextLevelIndex)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		return nullptr;
	}

	return pInstance;
}


