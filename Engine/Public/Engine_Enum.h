#pragma once
namespace Engine
{
	enum class WINMODE { FULL, WIN };

	// GameObject의 생명주기 상태
	enum class LIFESTATE { NONE, AWAKED, STARTED, REMOVE, END };

	enum class RENDERGROUP : uint8 { PRIORITY, NONBLEND, BLEND, UI, END };

	//// Dynamic 컴포넌트 경우 매 프레임마다 갱신해야하는 컴포넌트 집단
	//enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

	//enum INFO {	INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };

	//enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

	//enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_END };

	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };

	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

}