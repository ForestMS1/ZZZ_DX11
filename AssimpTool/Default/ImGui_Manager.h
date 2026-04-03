#pragma once

NS_BEGIN(AssimpTool)

class ImGui_Manager
{
	DECLARE_SINGLETON(ImGui_Manager)
private:
	ImGui_Manager();
public:
	~ImGui_Manager();

public:
	HRESULT Initialize(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext, HWND hwnd);
	void Update();
	void Render();

	static void Release();
};

NS_END
