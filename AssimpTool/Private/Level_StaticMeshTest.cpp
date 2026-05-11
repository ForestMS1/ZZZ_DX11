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
#include "TestUI.h"
#include "SkyBox.h"
#include "NetworkManager.h"
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
	GAME.Register(L"TestUI", []() {  auto obj = make_shared<TestUI>(DEVICE, CONTEXT); obj->Initialize(); return obj; });
	// 빈 오브젝트
	GAME.Register(L"GameObject", []() {  auto obj = make_shared<GameObject>(DEVICE, CONTEXT); obj->Initialize(); return obj; });

	if (FAILED(Ready_Layer_Basic(L"Layer_Basic")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(L"Layer_Camera")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(L"Layer_UI")))
		return E_FAIL;

	// 충돌처리 할 레이어 쌍 정의
	GAME.AddCollisionLayer(ETOUI(LEVEL::TESTMESH), L"Layer_Basic", L"Layer_Basic");
	//GAME.AddCollisionLayer(ETOUI(LEVEL::TESTMESH), L"Layer_Basic", L"Layer_Camera");


	NetworkManager::Get();
	NetworkManager::Get().RequestEnterScene("TestCorin");
	return S_OK;
}

void Level_StaticMeshTest::Update()
{
	GAME.ShowResourceList();
	GAME.ShowHiearchy();
	GAME.ShowInspector();
	GAME.RenderGizmo();
	

	//// 리소스 전부 해제하면서 씬 전환
	//if (GAME.Key_Down(DIK_BACKSPACE))
	//	GAME.Change_Level(ETOUI(LEVEL::LOADING), Level_Loading::Create(_device, _deviceContext, LEVEL::CONVERT), true);


	//if (GAME.Key_Down(DIK_BACKSPACE))
	//	NetworkManager::Get().RequestEnterScene("TestCorin");
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
	GAME.LoadLevel(ETOUI(LEVEL::TESTMESH), strLayerTag);

	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestCorin",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;
	//
	//
	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_SkyBox",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT Level_StaticMeshTest::Ready_Layer_Camera(const wstring& strLayerTag)
{
	GAME.Add_Layer(ETOUI(LEVEL::TESTMESH), strLayerTag);
	GAME.LoadLevel(ETOUI(LEVEL::TESTMESH), strLayerTag);

	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestCam",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;
	//
	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestPlayCam",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT Level_StaticMeshTest::Ready_Layer_UI(const wstring& strLayerTag)
{
	GAME.Add_Layer(ETOUI(LEVEL::TESTMESH), strLayerTag);
	GAME.LoadLevel(ETOUI(LEVEL::TESTMESH), strLayerTag);

	//if (FAILED(GameInstance::Get().Add_GameObject_toLayer(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestSphere",
	//	ETOUI(LEVEL::TESTMESH), strLayerTag)))
	//	return E_FAIL;
	//
	//auto ui = make_shared<TestUI>(DEVICE, CONTEXT);
	//ui->Initialize();
	//GAME.Add_GameObject_toLayerNoClone(ETOUI(LEVEL::TESTMESH), strLayerTag, ui);
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