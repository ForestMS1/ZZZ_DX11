#pragma once
#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND		hWnd;
		WINMODE		eWinMode;
		int32_t		iWinSizeX, iWinSizeY;
		int32_t		iNumLevels;
	}ENGINE_DESC;

}