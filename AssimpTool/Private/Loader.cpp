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
#include "NetworkView.h"
#include "NetworkTransformView.h"
#include "NetworkAnimationView.h"
#include "CorinWeaponScript.h"
#include "EllenWeaponScript.h"
#include "CorinStateMachineScript.h"
#include "EllenStateMachineScript.h"
#include "TagManagerScript.h"
#include "AlpecaInfestedScript.h"
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

	if (_isFinishedStaticLoading == false)
	{
		if (FAILED(Loading_For_Static()))
		{
			LeaveCriticalSection(&_criticalSection);
			return E_FAIL;
		}
	}

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



	_loadingText = L"모델을 로딩 중 입니다.";
	



	_loadingText = L"셰이더를 로딩 중 입니다.";
	



	_loadingText = L"객체원형 생성 중 입니다.";


	_loadingText = L"로딩이 완료되었습니다.";



	//_isFinished = true;
	_isFinishedStaticLoading = true;
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

	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/SkillNormal.dds");
		GAME.AddResource(L"SkillNormal", texture);
	}

	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/SkillEvade.dds");
		GAME.AddResource(L"SkillEvade", texture);
	}

	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/SkillSpecial.dds");
		GAME.AddResource(L"SkillSpecial", texture);
	}

	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/SkillSwitch.dds");
		GAME.AddResource(L"SkillSwitch", texture);
	}

	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/SkillBg.dds");
		GAME.AddResource(L"SkillBg", texture);
	}

	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/SkillBtnUlt.dds");
		GAME.AddResource(L"SkillBtnUlt", texture);
	}

	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/MouseLIcon.png");
		GAME.AddResource(L"MouseLIcon", texture);
	}

	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/MouseRIcon.png");
		GAME.AddResource(L"MouseRIcon", texture);
	}

	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/RoleHPFill01.png");
		GAME.AddResource(L"RoleHPFill01", texture);
	}
	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/RoleHPFill02.png");
		GAME.AddResource(L"RoleHPFill02", texture);
	}
	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/HpBarBg3.png");
		GAME.AddResource(L"HpBarBg3", texture);
	}
	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/HpBarBg2.png");
		GAME.AddResource(L"HpBarBg2", texture);
	}
	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/RoleSPFill01.png");
		GAME.AddResource(L"RoleSPFill01", texture);
	}
	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/RoleSPFill02.png");
		GAME.AddResource(L"RoleSPFill02", texture);
	}
	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/IconPauseBtn.png");
		GAME.AddResource(L"IconPauseBtn", texture);
	}
	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/IconRoleGeneral09.png");
		GAME.AddResource(L"IconRoleGeneral09", texture);
	}
	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/IconRoleGeneral21.png");
		GAME.AddResource(L"IconRoleGeneral21", texture);
	}
	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/IconRoleGeneral46.png");
		GAME.AddResource(L"IconRoleGeneral46", texture);
	}
	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/Eff_Assault_LC_06.png");
		GAME.AddResource(L"Eff_Assault_LC_06", texture);
	}

	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		vector<wstring> paths;
		for (int i = 0; i <= 9; ++i) // 0부터 9까지 다 채워넣기
		{
			wstring path = L"../../Resources/Textures/UI/Skill_0" + to_wstring(i) + L".png";
			paths.push_back(path);
		}
		texture->LoadTexture2DArray(paths);
		GAME.AddResource(L"Skill_0N", texture);
	}

	{
		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Load(L"../../Resources/Textures/UI/DamageTextPhysDmg.png");
		GAME.AddResource(L"DamageTextPhysDmg", texture);
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

	shared_ptr<Shader> ShadowAnimShader = Shader::Create(L"ShadowAnim.fx");
	GAME.AddResource<Shader>(L"ShadowAnim.fx", ShadowAnimShader);

	shared_ptr<Shader> ShadowNoAnimShader = Shader::Create(L"ShadowNoAnim.fx");
	GAME.AddResource<Shader>(L"ShadowNoAnim.fx", ShadowNoAnimShader);

	shared_ptr<Shader> BillBoardShader = Shader::Create(L"BillBoard.fx");
	GAME.AddResource<Shader>(L"BillBoard.fx", BillBoardShader);

	shared_ptr<Shader> ParticleShader = Shader::Create(L"Particle.fx");
	GAME.AddResource<Shader>(L"Particle.fx", ParticleShader);

	shared_ptr<Shader> DamageParticleShader = Shader::Create(L"DamageParticle.fx");
	GAME.AddResource<Shader>(L"DamageParticle.fx", DamageParticleShader);

	// ----------------------------------------------------Shader Load--------------------------------------------------------



	// ----------------------------------------------------Material Load--------------------------------------------------------
	_loadingText = L"머테리얼 로딩 중 입니다.";
	{
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
	}

	{
		shared_ptr<Material> material = make_shared<Material>();

		material->SetShader(SkyBoxShader);
		auto texture = GAME.GetOrAddTexture(L"Sky01", L"..\\..\\Resources\\Textures\\Sky01.jpg");
		material->SetDiffuseMap(texture);
		MaterialDesc& desc = material->GetMaterialDesc();
		desc.ambient = Vec4(1.f);
		desc.diffuse = Vec4(1.f);
		desc.specular = Vec4(1.f);

		GAME.AddResource(L"Sky01", material);
	}

	{
		shared_ptr<Material> material = make_shared<Material>();

		material->SetShader(BillBoardShader);
		auto texture = GAME.GetOrAddTexture(L"Sky01", L"..\\..\\Resources\\Textures\\Sky01.jpg");
		material->SetDiffuseMap(texture);
		MaterialDesc& desc = material->GetMaterialDesc();
		desc.ambient = Vec4(1.f);
		desc.diffuse = Vec4(1.f);
		desc.specular = Vec4(1.f);

		GAME.AddResource(L"Sky01BillBoard", material);
	}

	// ----------------------------------------------------Material Load--------------------------------------------------------



	// ----------------------------------------------------Model Load--------------------------------------------------------
	_loadingText = L"모델을 로딩 중 입니다.";

	// Corin 애니메이션 담은 모델
	shared_ptr<Model> CorinAnimModel = make_shared<Model>();
	CorinAnimModel->ReadModelRotatedY180(L"Corin/Corin");
	CorinAnimModel->ReadMaterial(L"Corin/Corin");
	CorinAnimModel->GetMaterialByIndex(0)->SetShader(TweenTestShader);

	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Idle");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Walk_Start");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Walk");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Run_Start");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Run");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Run_End");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Taunt");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchOut_Normal");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_Ex_Start");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_Ex");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_Ex_End");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_End");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_02");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_02_End");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_02_Explode");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_02_Explode_End");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_SwitchIn_Attack_02_Landed");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_ParryAid_Start");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_ParryAid_L");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_ParryAid_L_End");

	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_01");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_01_End");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_02");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_02_End");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_03");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_03_End");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_03_Explode");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_04");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_04_End");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_05");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_05_B");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Normal_05_End");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_AssaultAid");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_AssaultAid_End");


	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Rush");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Attack_Rush_End");



	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_QuestStart");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_LevelSwitch");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_HitFly_Front");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_HitFly_Back");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Hit_Shake");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Hit_L_Front");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Hit_L_Back");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Hit_H_Front");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Hit_H_Back");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Evade_Front");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Evade_Back");
	CorinAnimModel->ReadAnimationRotatedY180(L"Corin/Avatar_Female_Size01_Corin_Ani_Die");

	// Ellen 애니메이션 담은 모델
	shared_ptr<Model> EllenAnimModel = make_shared<Model>();
	EllenAnimModel->ReadModelRotatedY180(L"Ellen/Ellen");
	EllenAnimModel->ReadMaterial(L"Ellen/Ellen");
	EllenAnimModel->GetMaterialByIndex(0)->SetShader(TweenTestShader);

	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_QuestStart");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Idle");

	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk_Start");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk_Start_End");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Walk_End");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Run");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Run_End");


	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_SwitchIn_Attack_Ex_Start");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_SwitchIn_Normal");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_SwitchIn_Attack");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_SwitchIn_Attack_End");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_SwitchOut_Normal");

	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Evade_Front");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Evade_Back");

	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Rush");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Rush_End");

	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_ParryAid_Start");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_ParryAid_L");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_ParryAid_L_End");

	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_01_01");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_01_01_End");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_01_02");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_01_02_End");

	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_02_01");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_02_01_End");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_02_02");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_02_03");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_02_04");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_02_05");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_02_05_End");

	EllenAnimModel->ReadAnimation(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_03_01");
	EllenAnimModel->ReadAnimation(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_03_01_Back");
	EllenAnimModel->ReadAnimation(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_03_01_Back_Near");
	EllenAnimModel->ReadAnimation(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_03_01_End");
	EllenAnimModel->ReadAnimation(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_03_01_Near");
	EllenAnimModel->ReadAnimation(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_03_02");
	EllenAnimModel->ReadAnimation(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_03_02_Back");
	EllenAnimModel->ReadAnimation(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_03_02_End");
	EllenAnimModel->ReadAnimation(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_03_02_Near");
	EllenAnimModel->ReadAnimation(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Attack_Normal_03_02_Near_Back");

	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Hit_H_Back");
	EllenAnimModel->ReadAnimationRotatedY180(L"Ellen/Avatar_Female_Size02_Ellen_Ani_Hit_L_Back");

	// Alice 애니메이션 담은 모델
	shared_ptr<Model> AliceAnimModel = make_shared<Model>();
	AliceAnimModel->ReadModelRotatedY180(L"Alice/Alice");
	AliceAnimModel->ReadMaterial(L"Alice/Alice");
	AliceAnimModel->GetMaterialByIndex(0)->SetShader(TweenTestShader);

	AliceAnimModel->ReadAnimationRotatedY180(L"Alice/Avatar_Female_Size02_Alice_Ani_QuestStart");
	AliceAnimModel->ReadAnimationRotatedY180(L"Alice/Avatar_Female_Size02_Alice_Ani_Idle_Loop");

	//ExaPlaceBasement
	//shared_ptr<Model> StageModel = make_shared<Model>();
	//StageModel->ReadModel(L"ExaPlaceBasement/ExaPlaceBasement");
	//StageModel->ReadMaterial(L"ExaPlaceBasement/ExaPlaceBasement");
	//StageModel->GetMaterialByIndex(0)->SetShader(TestShader);
	//GAME.AddResource<Model>(L"ExaPlaceBasement", StageModel);

	shared_ptr<Model> VR2Model = make_shared<Model>();
	VR2Model->ReadModel(L"VR2/VR2");
	VR2Model->ReadMaterial(L"VR2/VR2");
	VR2Model->GetMaterialByIndex(0)->SetShader(TestShader);
	GAME.AddResource<Model>(L"VR2", VR2Model);

	// 몬스터 애니메이션 담은 모델
	//shared_ptr<Model> PalicusAnimModel = make_shared<Model>();
	//PalicusAnimModel->ReadModelRotatedY180(L"Monster_Palicus/Monster_Palicus");
	//PalicusAnimModel->ReadMaterial(L"Monster_Palicus/Monster_Palicus");
	//PalicusAnimModel->GetMaterialByIndex(0)->SetShader(TweenTestShader);
	//PalicusAnimModel->ReadAnimationRotatedY180(L"Monster_Palicus/Monster_Palicus_Ani_Attack_08_01");
	//PalicusAnimModel->ReadAnimationRotatedY180(L"Monster_Palicus/Monster_Palicus_Ani_Attack_08_02");
	//PalicusAnimModel->ReadAnimationRotatedY180(L"Monster_Palicus/Monster_Palicus_Ani_Attack_08_03");
	//PalicusAnimModel->ReadAnimationRotatedY180(L"Monster_Palicus/Death_Ani_Walk_L");
	//PalicusAnimModel->ReadAnimationRotatedY180(L"Monster_Palicus/Death_Ani_Walk_R");
	//PalicusAnimModel->CreateTexture();
	//GAME.AddResource<Model>(L"Monster_Palicus", PalicusAnimModel);

	// 몬스터 애니메이션 담은 모델
	shared_ptr<Model> Monster_AlpecaInfested_Model = make_shared<Model>();
	Monster_AlpecaInfested_Model->ReadModelRotatedY180(L"Monster_AlpecaInfested_Model/Monster_AlpecaInfested_Model");
	Monster_AlpecaInfested_Model->ReadMaterial(L"Monster_AlpecaInfested_Model/Monster_AlpecaInfested_Model");
	Monster_AlpecaInfested_Model->GetMaterialByIndex(0)->SetShader(TweenTestShader);
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Born");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Idle");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Walk_F");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Run_Start");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Run_Loop");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Run_End");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Death_Front");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Hit_H_Back");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Hit_H_Front");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Attack_01");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Attack_02");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Attack_03");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Attack_04");
	Monster_AlpecaInfested_Model->ReadAnimationRotatedY180(L"Monster_AlpecaInfested_Model/AlpecaInfested_Ani_Attack_05");

	// ----------------------------------------------------Model Load--------------------------------------------------------

	// 비동기(멀티스레드)로 본 행렬 연산 연산 시작
	_loadingText = L"모델 본 행렬을 계산 중 입니다.";
	
	std::vector<std::future<void>> futures;
	
	// std::launch::async 옵션을 주면 별도의 스레드가 파져서 동시에 실행.
	futures.push_back(std::async(std::launch::async, [=]() { CorinAnimModel->CreateTexture(); }));
	futures.push_back(std::async(std::launch::async, [=]() { EllenAnimModel->CreateTexture(); }));
	futures.push_back(std::async(std::launch::async, [=]() { AliceAnimModel->CreateTexture(); }));
	futures.push_back(std::async(std::launch::async, [=]() { Monster_AlpecaInfested_Model->CreateTexture(); }));
	
	// 모든 스레드가 본 계산을 완료할 때까지 대기
	for (auto& f : futures)
	{
		f.wait();
	}

	GAME.AddResource<Model>(L"CorinAnimModel", CorinAnimModel);
	GAME.AddResource<Model>(L"EllenAnimModel", EllenAnimModel);
	GAME.AddResource<Model>(L"AliceAnimModel", AliceAnimModel);
	GAME.AddResource<Model>(L"Monster_AlpecaInfested_Model", Monster_AlpecaInfested_Model);

	// ----------------------------------------------------Script Load--------------------------------------------------------

	_loadingText = L"스크립트 생성 중 입니다.";

	// 이름 Script로 시작해야 에디터에서 인식

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

	/* Script_CorinWeapon */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_CorinWeapon",
		CorinWeaponScript::Create())))
		return E_FAIL;

	/* Script_EllenWeapon */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_EllenWeapon",
		EllenWeaponScript::Create())))
		return E_FAIL;

	/* Script_CorinStateMachineScript */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_CorinStateMachineScript",
		CorinStateMachineScript::Create())))
		return E_FAIL;

	/* Script_EllenStateMachineScript */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_EllenStateMachineScript",
		EllenStateMachineScript::Create())))
		return E_FAIL;

	/* Script_TagManagerScript */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_TagManagerScript",
		TagManagerScript::Create())))
		return E_FAIL;

	/* Script_AlpecaInfestedScript */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_AlpecaInfestedScript",
		AlpecaInfestedScript::Create())))
		return E_FAIL;

	/* NetworkView */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_NetworkView",
		NetworkView::Create())))
		return E_FAIL;

	/* NetworkTransformView */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_NetworkTransformView",
		NetworkTransformView::Create())))
		return E_FAIL;

	/* NetworkAnimationView */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Script_NetworkAnimationView",
		NetworkAnimationView::Create())))
		return E_FAIL;
	// ----------------------------------------------------Script Load--------------------------------------------------------



	// ----------------------------------------------------GameObject Load--------------------------------------------------------
	_loadingText = L"객체원형 생성 중 입니다.";

	///* Prototype_GameObject_TestCam */
	//if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestCam",
	//	TestCam::Create(_device, _deviceContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_TestPlayCam */
	//if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestPlayCam",
	//	TestPlayCam::Create(_device, _deviceContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_TestCorin */
	//if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestCorin",
	//	TestCorin::Create(_device, _deviceContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_TestSphere */
	//if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestSphere",
	//	TestSphere::Create(_device, _deviceContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_SkyBox */
	//if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_SkyBox",
	//	SkyBox::Create(_device, _deviceContext))))
	//	return E_FAIL;

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