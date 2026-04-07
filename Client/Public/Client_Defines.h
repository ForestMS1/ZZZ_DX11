#pragma once
#include <Windows.h>
#include <memory>
#include <process.h>
#include "GameObject.h"

namespace Client
{
	static const unsigned int		g_iWinSizeX = { 1280 }; //1280 //1920
	static const unsigned int		g_iWinSizeY = { 720 }; //720  //1080

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
	inline const char* LEVEL_NAMES[] = { "STATIC", "LOADING", "LOGO", "GAMEPLAY", "END" };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

using namespace std;
using namespace Client;

//#define TIMER_60		L"Timer_60"
//#define DT				GAME.Get_TimeDelta(TIMER_60)