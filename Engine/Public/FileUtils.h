#pragma once

NS_BEGIN(Engine)
enum FileMode : uint8
{
	Write,
	Read
};

class ENGINE_DLL FileUtils
{
public:
	FileUtils();
	~FileUtils();

	void Open(const wstring& filePath, FileMode mode);

	template<typename T>
	void Write(const T& data)
	{
		DWORD numofBytes = 0;
		assert(::WriteFile(_handle, &data, sizeof(T), (LPDWORD)&numofBytes, nullptr));
	}

	template<>
	void Write<string>(const string& data)
	{
		return Write(data);
	}


	void Write(void* data, uint32 dataSize);
	void Write(const string& data);

	template<typename T>
	void Read(OUT T& data)
	{
		DWORD numofBytes = 0;
		assert(::ReadFile(_handle, &data, sizeof(T), (LPDWORD)&numofBytes, nullptr));
	}

	template<typename T>
	T Read()
	{
		T data;
		Read(data);
		return data;
	}

	void Read(void** data, uint32 dataSize);
	void Read(OUT string& data);

private:
	HANDLE _handle = INVALID_HANDLE_VALUE;

};

NS_END