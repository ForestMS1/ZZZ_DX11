#include "pch.h"
#include "ImGui_Manager.h"
#include "imnodes.h"

ImGui_Manager::ImGui_Manager()
{
}

ImGui_Manager::~ImGui_Manager()
{
}

HRESULT ImGui_Manager::Initialize(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext, HWND hwnd)
{

	IMGUI_CHECKVERSION();
	ImGuiContext* ctx = ImGui::CreateContext();
	ImNodesContext* nCtx = ImNodes::CreateContext();
	GAME.SetEngineContext(ctx, nCtx);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// 멀티 뷰포트 활성화 (창 밖으로 드래그 가능)
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// --- DPI 스케일링 핵심 부분 ---

	// 1. 현재 모니터의 DPI 계산 (기본값 96)
	//float dpi = GetDpiForWindow(hwnd);
	//float scale = dpi / 96.0f;

	// 2. 폰트 스케일링 (폰트 크기에 스케일 곱하기)
	// 기본 폰트는 스케일링이 안 되므로 외부 폰트(.ttf)를 로드해야 합니다.
	//ImFontConfig fontConfig;
	//fontConfig.SizePixels = 18.0f * scale; // 원하는 기본 크기 * 스케일
	//io.Fonts->AddFontDefault(&fontConfig);

	// 한글을 사용한다면 아래와 같이 로드
	// io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\malgun.ttf", 18.0f * scale, NULL, io.Fonts->GetGlyphRangesKorean());

	// 3. UI 스타일 스케일링 (버튼, 간격 등)
	ImGuiStyle& style = ImGui::GetStyle();
	//style.ScaleAllSizes(scale);

	// ----------------------------

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGuiStyle& style = ImGui::GetStyle();

	// 모든 창의 배경 투명도만 조절 (RGBA의 A값 수정)
	style.Colors[ImGuiCol_WindowBg].w = 0.4f;

	
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


	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport(0, viewport, flags);

	ImGuiIO& io = ImGui::GetIO();
}

void ImGui_Manager::Render()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// --- Multi-Viewport Update ---
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	//{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	//}
}


void ImGui_Manager::Release()
{
	// 1. 그래픽 백엔드 해제 (사용 중인 API에 맞춰 호출)
	ImGui_ImplDX11_Shutdown();  // DirectX 11 사용 시
	ImGui_ImplWin32_Shutdown(); // Win32 플랫폼 사용 시

	// 2. ImGui 컨텍스트 파괴 (메모리 해제)
	ImNodes::DestroyContext();
	ImGui::DestroyContext();
}