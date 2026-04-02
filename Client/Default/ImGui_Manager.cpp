#include "pch.h"
#include "ImGui_Manager.h"


ImGui_Manager::ImGui_Manager()
{
}

ImGui_Manager::~ImGui_Manager()
{
}

HRESULT ImGui_Manager::Initialize(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext, HWND hwnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(pDevice.Get(), pDeviceContext.Get());

	return S_OK;
}

void ImGui_Manager::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();
}

void ImGui_Manager::Render()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


void ImGui_Manager::Release()
{
	// 1. 그래픽 백엔드 해제 (사용 중인 API에 맞춰 호출)
	ImGui_ImplDX11_Shutdown();  // DirectX 11 사용 시
	ImGui_ImplWin32_Shutdown(); // Win32 플랫폼 사용 시

	// 2. ImGui 컨텍스트 파괴 (메모리 해제)
	ImGui::DestroyContext();
}