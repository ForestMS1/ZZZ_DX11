#include "pch.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Converter.h"
#include "TestCorin.h"
#include "TestCam.h"
#include "TestSphere.h"
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

	/* Prototype_GameObject_TestCam */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestCam",
		TestCam::Create(_device, _deviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_TestCorin */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestCorin",
		TestCorin::Create(_device, _deviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_TestSphere */
	if (FAILED(GameInstance::Get().Add_Prototype(ETOUI(LEVEL::TESTMESH), L"Prototype_GameObject_TestSphere",
		TestSphere::Create(_device, _deviceContext))))
		return E_FAIL;

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