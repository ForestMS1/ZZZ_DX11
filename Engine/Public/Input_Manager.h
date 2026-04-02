#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL Input_Manager
{
private:
	 Input_Manager();
public:
	virtual ~Input_Manager();

public:
	signed char	Get_DIKeyState(unsigned char byKeyID)
	{
		return _keyStates[byKeyID];
	}

	signed char	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return _mouseState.rgbButtons[eMouse];
	}

	// 현재 마우스의 특정 축 좌표를 반환
	signed long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((signed long*)&_mouseState) + eMouseState);
	}

	bool Key_Pressing(unsigned char byKeyID) { return _keyPressingStates[byKeyID]; }
	bool Key_Up(unsigned char byKeyID) { return _keyUpStates[byKeyID]; }
	bool Key_Down(unsigned char byKeyID) { return _keyDownStates[byKeyID]; }

	bool Mouse_Pressing(MOUSEKEYSTATE eMouseState) { return _mousePressingStates[eMouseState]; }
	bool Mouse_Up(MOUSEKEYSTATE eMouseState) { return _mouseUpStates[eMouseState]; }
	bool Mouse_Down(MOUSEKEYSTATE eMouseState) { return _mouseDownStates[eMouseState]; }

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	ComPtr<IDirectInput8W> _inputSDK = { nullptr };

private:
	ComPtr<IDirectInputDevice8W> _keyBoard = { nullptr };
	ComPtr<IDirectInputDevice8W> _mouse = { nullptr };

private:
	signed char					_keyStates[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE			_mouseState;

private:
	bool					_keyPressingStates[256];
	bool					_keyUpStates[256];
	bool					_keyDownStates[256];

	bool					_mousePressingStates[DIM_END];
	bool					_mouseUpStates[DIM_END];
	bool					_mouseDownStates[DIM_END];

public:
	static unique_ptr<Input_Manager> Create(HINSTANCE hInst, HWND hWnd);

};

NS_END