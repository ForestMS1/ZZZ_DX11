#include "pch.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Converter.h"
#include "TestCorin.h"
#include "TestCam.h"
#include "TestPlayCam.h"
#include "TestSphere.h"
#include "Transform.h"
#include "SkyBox.h"
#include "Texture.h"
#include "SpriteRenderer.h"

// --------------------Script-------------------
#include "TestCorinScript.h"
#include "TestCamScript.h"
#include "TestPlayCamScript.h"
// ---------------------------------------------
Loader::Loader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: _device(pDevice)
	, _deviceContext(pDeviceContext)
{
}

Loader::~Loader()
{
	WaitForSingleObject(_thread, INFINITE);

	CloseHandle(_thread);

	DeleteCriticalSection(&_criticalSection);
}

uint32 APIENTRY ThreadMain(void* pArg)
{
	auto pLoader = static_cast<Loader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}

HRESULT Loader::Initialize(LEVEL eNextLevelIndex)
{
	InitializeCriticalSection(&_criticalSection);

	_nextLevelIndex = eNextLevelIndex;

	_thread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);
	if (0 == _thread)
		return E_FAIL;

	return S_OK;
}

HRESULT Loader::Loading()
{
	EnterCriticalSection(&_criticalSection);

	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		LeaveCriticalSection(&_criticalSection);
		return E_FAIL;
	}

	HRESULT hr = {};

	switch (_nextLevelIndex)
	{
	case LEVEL::CONVERT:
		hr = Loading_For_Convert();
			break;
	case LEVEL::TESTMESH:
		hr = Loading_FOR_TestMesh();
		break;
	default:
		break;
	}

	if (FAILED(hr))
	{
		LeaveCriticalSection(&_criticalSection);
		return E_FAIL;
	}

	CoUninitialize();

	LeaveCriticalSection(&_criticalSection);

	return S_OK;
}

#ifdef _DEBUG
void Loader::Output_LoadingText()
{
	SetWindowText(g_hWnd, _loadingText.c_str());
}
#endif

HRESULT Loader::Loading_For_Static()
{
	_loadingText = L"텍스쳐를 로딩 중 입니다.";
	for (size_t i = 0; i < 99999999; i++)
	{
		int a = 10;
	}

	_loadingText = L"모델을 로딩 중 입니다.";
	for (size_t i = 0; i < 99999999; i++)
	{
		int a = 10;
	}
	_loadingText = L"셰이더를 로딩 중 입니다.";
	for (size_t i = 0; i < 99999999; i++)
	{
		int a = 10;
	}

	_loadingText = L"객체원형 생성 중 입니다.";


	_loadingText = L"로딩이 완료되었습니다.";

	_isFinished = true;

	return S_OK;
}

HRESULT Loader::Loading_For_Convert()
{
	_loadingText = L"텍스쳐를 로딩 중 입니다.";
	for (size_t i = 0; i < 99999999; i++)
	{
		int a = 10;
	}

	_loadingText = L"모델을 로딩 중 입니다.";
	for (size_t i = 0; i < 99999999; i++)
	{
		int a = 10;
	}
	_loadingText = L"셰이더를 로딩 중 입니다.";
	for (size_t i = 0; i < 99999999; i++)
	{
		int a = 10;
	}

	_loadingText = L"객체원형 생성 중 입니다.";

	_loadingText = L"로딩이 완료되었습니다.";

	_isFinished = true;

	return S_OK;
}

HRESULT Loader::Loading_FOR_TestMesh()
{
	// ----------------------------------------------------Texture Load--------------------------------------------------------
	_loadingText = L"텍스쳐를 로딩 중 입니다.";

	for (size_t i = 0; i < 38; ++i)
	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/SpriteTest/LogoBack_" + to_wstring(i) + L".png");
		GAME.AddResource(L"LogoBack_" + to_wstring(i), texture);
	}

	// ----------------------------------------------------Texture Load--------------------------------------------------------


	// ----------------------------------------------------Shader Load--------------------------------------------------------
	_loadingText = L"셰이더를 로딩 중 입니다.";

	// TODO : CONTEXT가 스레드Safe하지 않기 때문에 Initialize 위치를 메인스레드로 옮겨줘야 함
	shared_ptr<Shader> SkyBoxShader = Shader::Create(L"SkyBox.fx");
	GAME.AddResource<Shader>(L"SkyBox.fx", SkyBoxShader);

	shared_ptr<Shader> TweenTestShader = Shader::Create(L"TweenTest.fx");
	GAME.AddResource<Shader>(L"TweenTest.fx", TweenTestShader);

	shared_ptr<Shader> TestShader = Shader::Create(L"Test.fx");
	GAME.AddResource<Shader>(L"Test.fx", TestShader);

	shared_ptr<Shader> UIShader = Shader::Create(L"UI.fx");
	GAME.AddResource<Shader>(L"UI.fx", UIShader);

	// ----------------------------------------------------Shader Load--------------------------------------------------------



	// ----------------------------------------------------Material Load--------------------------------------------------------
	_loadingText = L"머테리얼 로딩 중 입니다.";
	shared_ptr<Material> material = make_shared<Material>();
	shared_ptr<Shader> shader = GAME.GetResource<Shader>(L"SkyBox.fx");

	material->SetShader(shader);
	auto texture = GAME.GetOrAddTexture(L"Sky", L"..\\..\\Resources\\Textures\\Sky.jpg");
	material->SetDiffuseMap(texture);
	MaterialDesc& desc = material->GetMaterialDesc();
	desc.ambient = Vec4(1.f);
	desc.diffuse = Vec4(1.f);
	desc.specular = Vec4(1.f);

	GAME.AddResource(L"Sky", material);

	// ----------------------------------------------------Material Load--------------------------------------------------------



	// ----------------------------------------------------Model Load--------------------------------------------------------
	_loadingText = L"모델을 로딩 중 입니다.";

	// Corin 기본 T자 모델
	shared_ptr<Model> CorinModel = make_shared<Model>();
	CorinModel->ReadModelRotatedY180(L"Corin/Corin");
	CorinModel->ReadMaterial(L"Corin/Corin");
	CorinModel->GetMaterialByIndex(0)->SetShader(TestShader);
	GAME.AddResource<Model>(L"CorinModel", CorinModel);

	// Corin 애니메이션 담은 모델
	shared_ptr<Model> CorinAnimModel = make_shared<Model>();
	CorinAnimModel->ReadModelRotatedY180(L"Corin/Corin");
	CorinAnimModel->ReadMaterial(L"Corin/Corin");
	CorinAnimModel->GetMaterialByIndex(0)->SetShader(TweenTestShader);

	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Idle");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Walk_Start");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Walk");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Run_Start");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Run");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Run_End");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Taunt");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchOut_Normal");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_Ex_Start");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_Ex");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_Ex_End");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_02");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_02_End");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_02_Explode");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_02_Explode_End");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_02_Landed");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_QuestStart");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_LevelSwitch");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_HitFly_Front");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_HitFly_Back");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Hit_Shake");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Hit_L_Front");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Hit_L_Back");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Hit_H_Front");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Hit_H_Back");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Evade_Front");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Evade_Back");
	CorinAnimModel->ReadAnimationRotatedY180NoMove(L"Corin/Avatar_Female_Size01_Corin_Ani_Die");
	GAME.AddResource<Model>(L"CorinAnimModel", CorinAnimModel);

	// Ellen 기본 T자 모델
	shared_ptr<Model> EllenModel = make_shared<Model>();
	EllenModel->ReadModelRotatedY180(L"Ellen/Ellen");
	EllenModel->ReadMaterial(L"Ellen/Ellen");
	EllenModel->GetMaterialByIndex(0)->SetShader(TestShader);
	GAME.AddResource<Model>(L"EllenModel", EllenModel);

	// Ellen 애니메이션 담은 모델
	shared_ptr<Model> EllenAnimModel = make_shared<Model>();
	EllenAnimModel->ReadModelRotatedY180(L"Ellen/Ellen");
	EllenAnimModel->ReadMaterial(L"Ellen/Ellen");
	EllenAnimModel->GetMaterialByIndex(0)->SetShader(TweenTestShader);

	EllenAnimModel->ReadAnimationRotatedY180NoMove(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk_Start");
	EllenAnimModel->ReadAnimationRotatedY180NoMove(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk_Start_End");
	EllenAnimModel->ReadAnimationRotatedY180NoMove(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk");
	EllenAnimModel->ReadAnimationRotatedY180NoMove(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk_End");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_SwitchIn_Attack_Ex_Start");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_SwitchIn_Normal");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_SwitchOut_Normal");
	GAME.AddResource<Model>(L"EllenAnimModel", EllenAnimModel);

	// Stage
	//shared_ptr<Model> StageModel = make_shared<Model>();
	//StageModel->ReadModelCombined(L"StageRoot/StageRoot");
	//StageModel->ReadMaterial(L"StageRoot/StageRoot");
	//StageModel->GetMaterialByIndex(0)->SetShader(TestShader);
	//GAME.AddResource<Model>(L"Stage", StageModel);

	//ExaPlaceBasement
	shared_ptr<Model> StageModel = make_shared<Model>();
	StageModel->ReadModel(L"ExaPlaceBasement/ExaPlaceBasement");
	StageModel->ReadMaterial(L"ExaPlaceBasement/ExaPlaceBasement");
	StageModel->GetMaterialByIndex(0)->SetShader(TestShader);
	GAME.AddResource<Model>(L"ExaPlaceBasement", StageModel);

	shared_ptr<Model> VR2Model = make_shared<Model>();
	VR2Model->ReadModel(L"VR2/VR2");
	VR2Model->ReadMaterial(L"VR2/VR2");
	VR2Model->GetMaterialByIndex(0)->SetShader(TestShader);
	GAME.AddResource<Model>(L"VR2", VR2Model);

	//shared_ptr<Model> StageRoot = make_shared<Model>();
	//StageRoot->ReadModel(L"StageRoot/StageRoot");
	//StageRoot->ReadMaterial(L"StageRoot/StageRoot");
	//StageRoot->GetMaterialByIndex(0)->SetShader(TestShader);
	//GAME.AddResource<Model>(L"StageRoot", StageRoot);

	//StageModel = make_shared<Model>();
	//StageModel->ReadModel(L"MetroCol/MetroCol");
	//StageModel->ReadMaterial(L"MetroCol/MetroCol");
	//StageModel->GetMaterialByIndex(0)->SetShader(TestShader);
	//GAME.AddResource<Model>(L"MetroCol", StageModel);

	// 몬스터 애니메이션 담은 모델
	shared_ptr<Model> PalicusAnimModel = make_shared<Model>();
	PalicusAnimModel->ReadModelRotatedY180(L"Monster_Palicus/Monster_Palicus");
	PalicusAnimModel->ReadMaterial(L"Monster_Palicus/Monster_Palicus");
	PalicusAnimModel->GetMaterialByIndex(0)->SetShader(TweenTestShader);
	PalicusAnimModel->ReadAnimationRotatedY180(L"Monster_Palicus/Monster_Palicus_Ani_Attack_08_01");
	PalicusAnimModel->ReadAnimationRotatedY180(L"Monster_Palicus/Monster_Palicus_Ani_Attack_08_02");
	PalicusAnimModel->ReadAnimationRotatedY180(L"Monster_Palicus/Monster_Palicus_Ani_Attack_08_03");
	PalicusAnimModel->ReadAnimationRotatedY180(L"Monster_Palicus/Death_Ani_Walk_L");
	PalicusAnimModel->ReadAnimationRotatedY180(L"Monster_Palicus/Death_Ani_Walk_R");
	GAME.AddResource<Model>(L"Monster_Palicus", PalicusAnimModel);

	// ----------------------------------------------------Model Load--------------------------------------------------------

	// ----------------------------------------------------Script Load--------------------------------------------------------

	_loadingText = L"스크립트 생성 중 입니다.";
	/* Script_TestCorin */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_TestCorin",
		TestCorinScript::Create())))
		return E_FAIL;

	/* Script_TestCam */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_TestCam",
		TestCamScript::Create())))
		return E_FAIL;

	/* Script_TestPlayCam */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_TestPlayCam",
		TestPlayCamScript::Create())))
		return E_FAIL;

	// ----------------------------------------------------Script Load--------------------------------------------------------



	// ----------------------------------------------------GameObject Load--------------------------------------------------------
	_loadingText = L"객체원형 생성 중 입니다.";

	/* Prototype_GameObject_TestCam */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestCam",
		TestCam::Create(_device, _deviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_TestPlayCam */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestPlayCam",
		TestPlayCam::Create(_device, _deviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_TestCorin */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestCorin",
		TestCorin::Create(_device, _deviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_TestSphere */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestSphere",
		TestSphere::Create(_device, _deviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_SkyBox */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_SkyBox",
		SkyBox::Create(_device, _deviceContext))))
		return E_FAIL;

	// ----------------------------------------------------GameObject Load--------------------------------------------------------
	_loadingText = L"로딩이 완료되었습니다.";

	_isFinished = true;
	return S_OK;
}

unique_ptr<Loader> Loader::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext, LEVEL eNextLevelIndex)
{
	auto pInstance = unique_ptr<Loader>(new Loader(pDevice, pDeviceContext));

	if (FAILED(pInstance->Initialize(eNextLevelIndex)))
	{
		MSG_BOX("Failed to Created : CLoader");
		return nullptr;
	}

	return pInstance;
}