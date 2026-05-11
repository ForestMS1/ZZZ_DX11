#include "pch.h"
#include "Utils.h"

bool Utils::StartsWith(const string& str, const string& comp)
{
	wstring::size_type index = str.find(comp);
	if (index != wstring::npos && index == 0)
		return true;

	return false;
}

bool Utils::StartsWith(const wstring& str, const wstring& comp)
{
	wstring::size_type index = str.find(comp);
	if (index != wstring::npos && index == 0)
		return true;

	return false;
}

void Utils::Replace(OUT string& str, const string& comp, const string& rep)
{
	string temp = str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	str = temp;
}

void Utils::Replace(OUT wstring& str, const wstring& comp, const wstring& rep)
{
	wstring temp = str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	str = temp;
}

std::wstring Utils::ToWString(const string& value)
{
	return wstring(value.begin(), value.end());
}

std::string Utils::ToString(const wstring& value)
{
	return string(value.begin(), value.end());
}

uint32_t Utils::GUIDToUint32(const GUID& guid)
{
	// GUID의 16바이트 데이터를 문자열 뷰나 바이트 배열로 취급
	// 여기서는 간단하게 std::hash를 활용
	std::hash<std::string_view> hasher;
	std::string_view data(reinterpret_cast<const char*>(&guid), sizeof(GUID));

	size_t hashed = hasher(data);

	// size_t(8바이트)를 uint32_t(4바이트)로 압축 (상위/하위 XOR)
#if defined(_WIN64)
	return static_cast<uint32_t>(hashed ^ (hashed >> 32));
#else
	return static_cast<uint32_t>(hashed);
#endif
}
