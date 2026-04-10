#include "pch.h"
#include "Level_StaticMeshTest.h"
#include "TestCorin.h"
#include "Model.h"
#include "Shader.h"
#include "ModelRenderer.h"
#include "TestCam.h"
#include "TestPlayCam.h"
#include "Camera.h"
#include "Level_Loading.h"

#include "TestSphere.h"
#include "SkyBox.h"
Level_StaticMeshTest::Level_StaticMeshTest(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: Level(pDevice, pDeviceContext)
{
}

Level_StaticMeshTest::~Level_StaticMeshTest()
{

}

HRESULT Level_StaticMeshTest::Initialize()
{
	GAME.Register(L"TestCam", [=]()  { auto obj = make_shared<TestCam>(DEVICE, CONTEXT); obj->Initialize(); return obj; } );
	GAME.Register(L"TestPlayCam", []() {  auto obj = make_shared<TestPlayCam>(DEVICE, CONTEXT); obj->Initialize(); return obj; });
	GAME.Register(L"SkyBox", []() {  auto obj = make_shared<SkyBox>(DEVICE, CONTEXT); obj->Initialize(); return obj; });
	GAME.Register(L"TestCorin", []() { auto obj = make_shared<TestCorin>(DEVICE, CONTEXT); obj->Initialize(); return obj; });
	GAME.Register(L"TestSphere", []() {  auto obj = make_shared<TestSphere>(DEVICE, CONTEXT); obj->Initialize(); return obj; });

	if (FAILED(Ready_Layer_Basic(L"Layer_Basic")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Test(L"Layer_Test")))
		return E_FAIL;


	return S_OK;
}

void Level_StaticMeshTest::Update()
{
	GAME.ShowResourceList();
	GAME.ShowHiearchy();
	GAME.ShowInspector();
	GAME.RenderGizmo();

	// 리소스 전부 해제하면서 씬 전환
	if (GAME.Key_Down(DIK_RETURN))
		GAME.Change_Level(ETOUI(LEVEL::LOADING), Level_Loading::Create(_device, _deviceContext, LEVEL::CONVERT), true);
}

HRESULT Level_StaticMeshTest::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, L"정적메쉬 테스트 레벨임.");
#endif

	return S_OK;
}

HRESULT Level_StaticMeshTest::Ready_Layer_Basic(const wstring& strLayerTag)
{
	GAME.Add_Layer(ETOUI(LEVEL::TESTMESH), strLayerTag);
	//GAME.LoadLevel(ETOUI(LEVEL::TESTMESH), strLayerTag);
	
	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestCam",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;
	
	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestPlayCam",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;

	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestCorin",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;

	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestSphere",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;

	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestSphere",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;

	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_SkyBox",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT Level_StaticMeshTest::Ready_Layer_Test(const wstring& strLayerTag)
{
	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestCorin",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;
	return S_OK;
}

unique_ptr<Level_StaticMeshTest> Level_StaticMeshTest::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	auto pInstance = unique_ptr<Level_StaticMeshTest>(new Level_StaticMeshTest(pDevice, pDeviceContext));
	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(g_hWnd, TEXT("Failed to Created : Level_StaticMeshTest"), nullptr, MB_OK);
		return nullptr;
	}

	return pInstance;
}