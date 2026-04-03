#pragma once

NS_BEGIN(Engine)

class ImGui_Manager
{
private:
	ImGui_Manager();
public:
	~ImGui_Manager();

public:
	HRESULT Initialize(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext, HWND hwnd);
	void Update();
	void Render();

	static unique_ptr<ImGui_Manager> Create(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext, HWND hwnd);
	static void Release();

};

NS_END
