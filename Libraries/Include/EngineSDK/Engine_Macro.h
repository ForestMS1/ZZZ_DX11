#pragma once


#define			SHARED_THIS(_type) static_pointer_cast<_type>(shared_from_this())

#define			ETOI(_enum)			static_cast<int32_t>(_enum)
#define			ETOUI(_enum)		static_cast<uint32_t>(_enum)

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			NS_BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			NS_END						}

#define			NS_USING(NAMESPACE)	using namespace NAMESPACE;

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif


#define NULL_CHECK( _ptr)	\
		{if( _ptr == 0){ return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
		{if( _ptr == 0){return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
		{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
		{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);return _return;}}

#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK);  return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
		{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK);  return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, _message, L"System Message",MB_OK); return E_FAIL;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, _message, L"System Message",MB_OK); return _return;}



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

#define GET(CLASSNAME)	CLASSNAME::Get()

#define GAME			GET(GameInstance)