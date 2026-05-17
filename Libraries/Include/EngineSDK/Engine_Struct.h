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

	// 필요하면 구조체 상속
	struct ENGINE_DLL EventDesc
	{
		void* sender;
		uint32 eventType;
		virtual ~EventDesc() = default;
	};
}