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

	static uint32_t GUIDToUint32(const GUID& guid);

    static constexpr uint32_t OFFSET_BASIS = 2166136261u;
    static constexpr uint32_t PRIME = 16777619u;

    // wstring 지원 버전
    static uint32_t Hash(const std::wstring& str) {
        uint32_t hash = OFFSET_BASIS;
        for (wchar_t c : str) {
            // wchar_t의 각 바이트를 처리 (2바이트씩)
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&c);
            for (int i = 0; i < sizeof(wchar_t); ++i) {
                hash ^= bytes[i];
                hash *= PRIME;
            }
        }
        return hash;
    }
};

NS_END