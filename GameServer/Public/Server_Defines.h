#pragma once
#define WIN32_LEAN_AND_MEAN

#include "Server_Typedef.h"


#include "Server_Enum.h"
#include "Server_Macro.h"
#include "Server_Struct.h"
#include "Server_Function.h"

using namespace DirectX;
using namespace Server;

#define NO_COPY(CLASSNAME)											\
			private:												\
			CLASSNAME(const CLASSNAME&) = delete;					\
			CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)								\
			NO_COPY(CLASSNAME)										\
		public:														\
			static CLASSNAME& Get(void) {							\
			static CLASSNAME Instance;								\
			return Instance;										\
		}

