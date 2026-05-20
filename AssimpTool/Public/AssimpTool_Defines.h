#pragma once
#include <Windows.h>
#include <memory>
#include <process.h>

#include "GameObject.h"

namespace AssimpTool
{
	static const unsigned int		g_iWinSizeX = { 1280 }; //1280 //1920
	static const unsigned int		g_iWinSizeY = { 720 };	//720  //1080

	enum class LEVEL { STATIC, LOADING, CONVERT, TESTMESH, END };
	inline const char* LEVEL_NAMES[] = { "STATIC", "LOADING", "CONVERT", "TESTMESH", "END" };
	
	enum class EventType { LEVEL_START, CHARACTER_SWITCH, BOSS_SPAWN, WIPEOUT, END };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

using namespace std;
using namespace AssimpTool;

//#define TIMER_60		L"Timer_60"
//#define DT				GAME.Get_TimeDelta(TIMER_60)