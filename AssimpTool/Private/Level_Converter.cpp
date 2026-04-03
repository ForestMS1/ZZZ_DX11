#include "pch.h"
#include "Level_Converter.h"

#include "GameInstance.h"
#include "Converter.h"

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
	converter->ReadAssetFile(L"Corin/Avatar_Female_Size01_Corin_Model.fbx");
	converter->ExportMaterialData(L"Corin/Corin");
	converter->ExportModelData(L"Corin/Corin");
	converter->ExportAnimationDataAll(L"Corin");

	return S_OK;
}

void Level_Converter::Update()
{

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
