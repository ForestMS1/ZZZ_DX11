#include "pch.h"
#include "Input_Manager.h"


Engine::Input_Manager::Input_Manager(void)
{
	ZeroMemory(_keyStates, sizeof(_keyStates));
	ZeroMemory(&_mouseState, sizeof(_mouseState));

	ZeroMemory(_keyUpStates, sizeof(_keyUpStates));
	ZeroMemory(_keyDownStates, sizeof(_keyDownStates));
	ZeroMemory(_keyPressingStates, sizeof(_keyPressingStates));

	ZeroMemory(_mouseUpStates, sizeof(_mouseUpStates));
	ZeroMemory(_mouseDownStates, sizeof(_mouseDownStates));
	ZeroMemory(_mousePressingStates, sizeof(_mousePressingStates));
}

Engine::Input_Manager::~Input_Manager(void)
{
	
}

HRESULT Engine::Input_Manager::Initialize(HINSTANCE hInst, HWND hWnd)
{

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&_inputSDK,
		NULL)))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED(_inputSDK->CreateDevice(GUID_SysKeyboard, &_keyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	_keyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	_keyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	_keyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(_inputSDK->CreateDevice(GUID_SysMouse, &_mouse, nullptr)))
		return E_FAIL;


	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	_mouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	_mouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	_mouse->Acquire();


	return S_OK;
}

void Engine::Input_Manager::Update_InputDev(void)
{
	_keyBoard->GetDeviceState(256, _keyStates);
	_mouse->GetDeviceState(sizeof(_mouseState), &_mouseState);

	// 키 체킹
	{
		ZeroMemory(_keyUpStates, sizeof(_keyUpStates));
		ZeroMemory(_keyDownStates, sizeof(_keyDownStates));
		signed char keyDevState[256];
		_keyBoard->GetDeviceState(256, keyDevState);

		for (int i = 0; i < 256; ++i)
		{
			bool nowPressed = keyDevState[i] & 0x80;

			// 이전에 눌렸는데 지금 안눌렸으면 UP
			if (_keyPressingStates[i] && !nowPressed)
			{
				_keyUpStates[i] = true;
			}
			// 이전에 안 눌렸는데 지금 눌렸으면 DOWN
			else if (!_keyPressingStates[i] && nowPressed)
			{
				_keyDownStates[i] = true;
			}

			_keyPressingStates[i] = nowPressed;
		}
	}

	// 마우스 체킹
	{
		ZeroMemory(_mouseUpStates, sizeof(_mouseUpStates));
		ZeroMemory(_mouseDownStates, sizeof(_mouseDownStates));
		DIMOUSESTATE mouseDevState;
		_mouse->GetDeviceState(sizeof(mouseDevState), &mouseDevState);

		for (int i = 0; i < DIM_END; ++i)
		{
			bool nowPressed = mouseDevState.rgbButtons[i] & 0x80;

			// 이전에 눌렸는데 지금 안눌렸으면 UP
			if (_mousePressingStates[i] && !nowPressed)
			{
				_mouseUpStates[i] = true;
			}
			// 이전에 안 눌렸는데 지금 눌렸으면 DOWN
			else if (!_mousePressingStates[i] && nowPressed)
			{
				_mouseDownStates[i] = true;
			}

			_mousePressingStates[i] = nowPressed;
		}
	}
}

unique_ptr<Input_Manager> Input_Manager::Create(HINSTANCE hInst, HWND hWnd)
{
	auto pInstance = unique_ptr<Input_Manager>(new Input_Manager);
	if (FAILED(pInstance->Initialize(hInst, hWnd)))
	{
		MSG_BOX("Input_Manager Create Failed");
		return nullptr;
	}
	return pInstance;
}
