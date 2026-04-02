#include "pch.h"
#include "Loader.h"
#include "GameInstance.h"

#include "BackGround.h"
#include "TestCube.h"
#include "MainCam.h"
#include "Transform.h"

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
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
			break;
	case LEVEL::GAMEPLAY:
		hr = Loading_FOR_Game();
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

HRESULT Loader::Loading_For_Logo()
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
	/* Prototype_GameObject_MainCam */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_GameObject_MainCam",
		MainCam::Create(_device, _deviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_BackGround */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::LOGO), L"Prototype_GameObject_BackGround",
		BackGround::Create(_device, _deviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_TestCube */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::LOGO), L"Prototype_GameObject_TestCube",
		TestCube::Create(_device, _deviceContext))))
		return E_FAIL;

	_loadingText = L"로딩이 완료되었습니다.";

	_isFinished = true;

	return S_OK;
}

HRESULT Loader::Loading_FOR_Game()
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
	for (size_t i = 0; i < 99999999; i++)
	{
		int a = 10;
	}

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