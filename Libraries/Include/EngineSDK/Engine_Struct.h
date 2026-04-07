#pragma once
#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HINSTANCE	hInst;
		HWND		hWnd;
		WINMODE		eWinMode;
		int32_t		iWinSizeX, iWinSizeY;
		int32_t		iNumLevels;
		const char** ppLevelNames = nullptr;
	}ENGINE_DESC;

}