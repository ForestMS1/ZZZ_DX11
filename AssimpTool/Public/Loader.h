#pragma once

NS_BEGIN(AssimpTool)

class Loader final
{
private:
	Loader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
public:
	~Loader();

public:
	HRESULT Initialize(LEVEL eNextLevelIndex);
	HRESULT Loading();
	bool	IsFinished() const { return _isFinished; }

#ifdef _DEBUG
public:
	void Output_LoadingText();
#endif

private:
	LEVEL _nextLevelIndex = { LEVEL::END };
	ComPtr<ID3D11Device> _device = { nullptr };
	ComPtr<ID3D11DeviceContext> _deviceContext = { nullptr };

private: // For.Thread
	HANDLE		_thread = {};
	CRITICAL_SECTION _criticalSection = {};

private: // For.Loading
	bool		_isFinished = { false };
	wstring		_loadingText = {};

private:
	HRESULT Loading_For_Static();
	HRESULT Loading_For_Convert();
	HRESULT Loading_FOR_TestMesh();

public:
	static unique_ptr<Loader> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext, LEVEL eNextLevelIndex);
};

NS_END
