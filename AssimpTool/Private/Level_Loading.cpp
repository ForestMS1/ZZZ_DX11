#include "pch.h"
#include "Level_Loading.h"

#include "Loader.h"

#include "Level_StaticMeshTest.h"
#include "Level_Converter.h"
#include "GameInstance.h"

#include "Texture.h"

Level_Loading::Level_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: Level(pDevice, pDeviceContext)
{
	shared_ptr<Texture> texture = make_shared<Texture>();
	texture->Load(L"../../Resources/Textures/UI/Loading_FortuneSquare_Cam01.dds");
	GAME.AddResource<Texture>(L"Loading_FortuneSquare_Cam01", texture);
	
	_loadingBackground = make_shared<GameObject>(_device, _deviceContext);
	_loadingBackground->Initialize();
	_loadingBackground->SetName(L"Loading_Background");
	
	shared_ptr<Shader> UIShader = Shader::Create(L"UI.fx");
	GAME.AddResource<Shader>(L"UI.fx", UIShader);
	
	shared_ptr<SpriteRenderer> uiRenderer = make_shared<SpriteRenderer>();
	uiRenderer->SetShader(UIShader);
	uiRenderer->Add_Texture(texture);
	uiRenderer->SetUIWidth(2.f);
	uiRenderer->SetUIHeight(2.f);
	uiRenderer->SetUIPosX(0);
	uiRenderer->SetUIPosY(0);
	
	_loadingBackground->AddComponent(uiRenderer);
	
	_loadingText = make_shared<GameObject>(_device, _deviceContext);
	_loadingText->Initialize();
	_loadingText->SetName(L"Loading_Text");
	
	texture = make_shared<Texture>();
	texture->Load(L"../../Resources/Textures/UI/LoadingText.png");
	GAME.AddResource<Texture>(L"LoadingText", texture);
	
	uiRenderer = make_shared<SpriteRenderer>();
	uiRenderer->SetShader(UIShader);
	uiRenderer->Add_Texture(texture);
	uiRenderer->SetUIWidth(0.404f);
	uiRenderer->SetUIHeight(0.2f);
	uiRenderer->SetUIPosX(0.7f);
	uiRenderer->SetUIPosY(0.7f);
	_loadingText->AddComponent(uiRenderer);
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
	GAME.ShowResourceList();
	GAME.ShowHiearchy();
	GAME.ShowInspector();
	GAME.RenderGizmo();

	if (true == _loader->IsFinished() &&
		GAME.Key_Down(DIK_RETURN))
	{
		unique_ptr<Level> pNewLevel = { nullptr };

		switch (_nextLevelIndex)
		{
		case LEVEL::CONVERT:
			pNewLevel = Level_Converter::Create(_device, _deviceContext);
			break;
		case LEVEL::TESTMESH:
			pNewLevel = Level_StaticMeshTest::Create(_device, _deviceContext);
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
	GAME.Add_GameObject_toLayerNoClone(static_cast<uint32>(LEVEL::LOADING), strLayerTag, _loadingText);
	GAME.Add_GameObject_toLayerNoClone(static_cast<uint32>(LEVEL::LOADING), strLayerTag, _loadingBackground);

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


