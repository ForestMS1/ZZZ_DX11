#include "pch.h"
#include "Level_Converter.h"

#include "GameInstance.h"
#include "Converter.h"
#include "Level_Loading.h"
Level_Converter::Level_Converter(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: Level(pDevice, pDeviceContext)
{
}

Level_Converter::~Level_Converter()
{
}

HRESULT Level_Converter::Initialize()
{
	if (FAILED(Ready_Layer_Basic(L"Layer_Basic")))
		return E_FAIL;

	shared_ptr<Converter> converter = make_shared<Converter>();
	//converter->ReadAssetFile(L"Ellen/Avatar_Female_Size02_Ellen_Model.fbx");
	//converter->ExportMaterialData(L"Ellen/Ellen");
	//converter->ExportModelData(L"Ellen/Ellen");
	//converter->ExportAnimationDataAll(L"Ellen");

	//converter->ReadAssetFile(L"VR_1.fbx");
	//converter->ExportMaterialData(L"VR_1/VR_1");
	//converter->ExportModelData(L"VR_1/VR_1");


	return S_OK;
}

void Level_Converter::Update()
{
	GAME.ShowResourceList();
	GAME.ShowHiearchy();
	GAME.ShowInspector();
	GAME.RenderGizmo();

	if (GAME.Key_Down(DIK_RETURN))
		GAME.Change_Level(ETOUI(LEVEL::LOADING), Level_Loading::Create(_device, _deviceContext, LEVEL::TESTMESH),true);
}

HRESULT Level_Converter::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, L"컨버터 레벨임.");
#endif
	return S_OK;
}

HRESULT Level_Converter::Ready_Layer_Basic(const wstring& strLayerTag)
{
	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::STATIC), L"Prototype_GameObject_MainCam",
	//	ETOUI(LEVEL::LOGO), strLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

unique_ptr<Level_Converter> Level_Converter::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<Level_Converter>(new Level_Converter(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(g_hWnd, TEXT("Failed to Created : Level_Converter"), nullptr, MB_OK);
		return nullptr;
	}

	return pInstance;
}
