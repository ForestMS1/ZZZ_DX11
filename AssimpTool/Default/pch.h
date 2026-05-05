#pragma once


//#include "targetver.h"
// Windows 헤더 파일
#define _WIN32_WINNT 0x0A00 // Windows 10 타겟
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

// boost::asio
#include <iostream>
#include <thread>
#include <mutex>
#include <boost/version.hpp>
#include <boost/asio.hpp>

#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// ImGUI
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "Engine_Defines.h"
#include "AssimpTool_Defines.h"
#include "GameInstance.h"
#define _XM_NO_INTRINSICS_

#include "../../Common/CommonStruct.h"
// DPI 관련 헤더 추가
#include <ShellScalingApi.h>
// 라이브러리 링크 (프로젝트 설정에서 추가해도 되지만 이게 편합니다)
#pragma comment(lib, "Shcore.lib")