#pragma once
#include <Windows.h>
#include <memory>
#include <process.h>

namespace AssimpTool
{
	static const unsigned int		g_iWinSizeX = { 1280 };
	static const unsigned int		g_iWinSizeY = { 720 };

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

using namespace std;
using namespace AssimpTool;

#define TIMER_60		L"Timer_60"
#define DT				GAME.Get_TimeDelta(TIMER_60)