#pragma once

NS_BEGIN(Client)

class MainApp
{
private:
	MainApp();
public:
	~MainApp();

public:
	HRESULT Initialize();
	void	Update();
	HRESULT Render();

private:
	ComPtr<ID3D11Device> _device = { nullptr };
	ComPtr<ID3D11DeviceContext> _deviceContext = { nullptr };

private:
	HRESULT Start_Level(LEVEL eStartLevelIndex);

public:
	static unique_ptr<MainApp> Create();
};

NS_END