#pragma once
#define WIN32_LEAN_AND_MEAN
#include "tinyxml2.h" // <- windows헤더보다 위에 있어야함
#include "Engine_Typedef.h"

#include <vector>
#include <list>
#include <array>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <ctime>
#include <memory>
#include <wrl/client.h>
using namespace Microsoft::WRL;

using namespace std;

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

// VertexData
#include "VertexData.h"
#include "BindShaderDesc.h"

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#pragma warning(disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
using namespace DirectX;

#include <FX11/d3dx11effect.h>

// Assimp
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h" // 연산자 오버로딩이 정의되어 있음

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"
#include "ImGradient.h"
#include "ImCurveEdit.h"
#include "GraphEditor.h"

//uuid
#include <rpc.h>
#pragma comment(lib, "rpcrt4.lib")

// Libs
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex_debug.lib")
#pragma comment(lib, "FX11/Effects11d.lib")
#pragma comment(lib, "Assimp/assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#pragma comment(lib, "FX11/Effects11.lib")
#pragma comment(lib, "Assimp/assimp-vc143-mt.lib")
#endif

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

#include "Utils.h"
#include "GameInstance.h"


// ---------Resource-------------
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Model.h"
#include "ModelAnimation.h"
#include "ModelMesh.h"
// ---------Resource-------------

using namespace Engine;


