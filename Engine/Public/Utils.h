#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL Utils
{
public:
	static bool StartsWith(const string& str, const string& comp);
	static bool StartsWith(const wstring& str, const wstring& comp);

	static void Replace(OUT string& str, const string& comp, const string& rep);
	static void Replace(OUT wstring& str, const wstring& comp, const wstring& rep);

	static wstring ToWString(const string& value);
	static string ToString(const wstring& value);
};

NS_END