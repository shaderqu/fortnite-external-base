#pragma once
#include "overlay.h"
#include "../psef.h"
#define MYCLASSNAME    _("RealtekAudioBackgroundProcessClass")
#define MYWINDOWNAME   _("RealtekAudioBackgroundProcessClass")


using Overlay::DX11Overlay;

WNDCLASSEX DX11Overlay::wcex = { 0 };
MARGINS DX11Overlay::Margin = { -1 };
HINSTANCE DX11Overlay::InjectedModule;

HWND DX11Overlay::PSEFOverlay;
RECT DX11Overlay::Rect;
INT DX11Overlay::width;
INT DX11Overlay::height;
INT DX11Overlay::widthD2;
INT DX11Overlay::heightD2;

auto timeOld = std::chrono::high_resolution_clock::now();
auto timeNow = std::chrono::high_resolution_clock::now();
auto OverlayfpsClock = std::chrono::high_resolution_clock::now();

ID3D11Device* DX11Overlay::pDevice = nullptr;
ID3D11DeviceContext* DX11Overlay::PDeviceContext = nullptr;
IDXGISwapChain* DX11Overlay::pSwapChain = nullptr;
ID3D11RenderTargetView* DX11Overlay::pRenderTarget = nullptr;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT pMsg, WPARAM wParam, LPARAM lParam) {

	if (ImGui_ImplWin32_WndProcHandler(hWnd, pMsg, wParam, lParam) && settings::misc::show_menu)
		return TRUE;

	return DefWindowProc(hWnd, pMsg, wParam, lParam);
}

// Initialize ImGui
BOOL DX11Overlay::InitImGui() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig fontConfig;
	fontConfig.RasterizerMultiply = 1.4f;

	io.Fonts->AddFontFromFileTTF(_("C:\\Windows\\Fonts\\verdana.ttf"), 14.0f, &fontConfig, io.Fonts->GetGlyphRangesCyrillic());

	ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.00f);

	ImGui::StyleColorsDark();
	auto style = &ImGui::GetStyle();
	style->WindowRounding = 0;
	style->ChildRounding = 0;
	style->FrameRounding = 0;
	style->GrabRounding = 0;
	style->PopupRounding = 0; // Combobox

	style->ScrollbarSize = 9;
	style->FramePadding = ImVec2(0, 0);
	style->ItemSpacing = ImVec2(4, 4);

	// Initialize ImGui for Win32 and DX11
	if (ImGui_ImplWin32_Init(PSEFOverlay)) {
		if (ImGui_ImplDX11_Init(pDevice, PDeviceContext))
			return TRUE;
	}
	return FALSE;
}

// Function to create the overlay window
BOOL DX11Overlay::CreateOverlay() {
	// Set the properties for the window class extended structure
	wcex.cbSize = sizeof(wcex);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = InjectedModule;
	wcex.lpszClassName = MYCLASSNAME;

	// Register the window class
	if (!RegisterClassExA(&wcex))
		return FALSE;

	// Create the overlay window
	PSEFOverlay = CreateWindowExA(NULL, MYCLASSNAME, MYWINDOWNAME, WS_POPUP | WS_VISIBLE, 0, 0, width, height, 0, 0, InjectedModule, 0);

	// Check if the window was created successfully
	if (!PSEFOverlay)
		return FALSE;

	// Set the window style to make it layered, tool window, and topmost
	SetWindowLong(PSEFOverlay, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);

	// Extend the frame into the client area to remove the window border and title bar
	DwmExtendFrameIntoClientArea(PSEFOverlay, &Margin);

	// Show and update the overlay window
	ShowWindow(PSEFOverlay, SW_SHOW);
	UpdateWindow(PSEFOverlay);

	// Set the window position to the topmost
	SetWindowPos(PSEFOverlay, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// Set the window style to make it layered, tool window, topmost, and transparent
	SetWindowLongA(PSEFOverlay, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT);
	SetWindowLong(PSEFOverlay, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW);

	return TRUE;
}

BOOL DX11Overlay::CreateRenderTarget() {
	ID3D11Texture2D* RenderBuffer = nullptr;
	if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&RenderBuffer))))
	{
		pDevice->CreateRenderTargetView(RenderBuffer, NULL, &pRenderTarget);
		RenderBuffer->Release();
	}
	return TRUE;
}

inline ImFont* icons;
inline ImFont* font_main;

// Function to create the Direct3D 11 swap chain
BOOL DX11Overlay::CreateSwapchain() {
	// Set the properties for the DXGI swap chain
	DXGI_SWAP_CHAIN_DESC SwapChainDC{};
	ZeroMemory(&SwapChainDC, sizeof(SwapChainDC));
	SwapChainDC.Windowed = TRUE;
	SwapChainDC.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDC.BufferCount = 2;
	SwapChainDC.BufferDesc.Width = 0;
	SwapChainDC.BufferDesc.Height = 0;
	SwapChainDC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDC.OutputWindow = PSEFOverlay;
	SwapChainDC.SampleDesc.Count = 1;
	SwapChainDC.SampleDesc.Quality = DEFAULT_QUALITY;
	SwapChainDC.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDC.BufferDesc.RefreshRate.Numerator = 240;
	SwapChainDC.BufferDesc.RefreshRate.Denominator = 1;

	// Set the Direct3D feature level and create the device and swap chain
	D3D_FEATURE_LEVEL vFeatureLevel;
	D3D_FEATURE_LEVEL vFeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

	HRESULT create = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, vFeatureLevelArray, 2, D3D11_SDK_VERSION, &SwapChainDC, &pSwapChain, &pDevice, &vFeatureLevel, &PDeviceContext);
	if (create)
		return FALSE;

	// Create the render target view
	if (!CreateRenderTarget())
		return FALSE;

	// Set the render target for rendering
	PDeviceContext->OMSetRenderTargets(1, &pRenderTarget, NULL);
	return TRUE;
}

// Function to check if the GameWindow handle is valid
VOID DX11Overlay::ChechWindow(HWND GameWindow) {
	if (!GameWindow) TerminateProcess(GetCurrentProcess(), 3);
}

// Function to get the position and size of the GameWindow
VOID DX11Overlay::GetWindow(HWND GameWindow) {
	if (GetWindowRect(GameWindow, &Rect)) {
		width = Rect.right - Rect.left;
		height = Rect.bottom - Rect.top;

		widthD2 = width / 2;
		heightD2 = height / 2;
	}
}

// Function to initialize the overlay and associated components
BOOL DX11Overlay::InitOverlay() {
	if (!CreateOverlay()) {
		std::cout << _("\n [ - ] Failed to create overlay!");
		return FALSE;
	}
	if (!CreateSwapchain()) {
		std::cout << _("\n [ - ] Failed to create swapchain!");
		return FALSE;
	}
	if (!InitImGui()) {
		std::cout << _("\n [ - ] Failed to initialize swapchain!");
		return FALSE;
	}
	return TRUE;
}

// Function to clean up and present the overlay
BOOL DX11Overlay::Cleanup() {
	static ImVec4 vCleanColor{ 0, 0, 0, 0 };

	// Render ImGui
	ImGui::Render();
	ImGui::EndFrame();

	// Clear the render target view with the specified clean color
	PDeviceContext->ClearRenderTargetView(pRenderTarget, (float*)&vCleanColor);

	// Render ImGui draw data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Present the swap chain
	HRESULT pSent = pSwapChain->Present(1, 0);
	if (pSent)
		return TRUE;
	return FALSE;
}

// Function to change the clickability of the overlay window
bool clickable = false;
void change_click(bool canclick) {
	long style = ::GetWindowLong(DX11Overlay::PSEFOverlay, GWL_EXSTYLE);
	if (canclick)
	{
		style &= ~WS_EX_LAYERED;
		SetWindowLong(DX11Overlay::PSEFOverlay, GWL_EXSTYLE, style);
		SetFocus(DX11Overlay::PSEFOverlay);
		clickable = true;
	}
	else
	{
		style |= WS_EX_LAYERED;
		SetWindowLong(DX11Overlay::PSEFOverlay, GWL_EXSTYLE, style);
		SetFocus(DX11Overlay::PSEFOverlay);
		clickable = false;
	}
}

// Function to run the main overlay loop
BOOL DX11Overlay::MainLoop(std::function<VOID()> DrawLoop) {
	MSG Msg;
	static ImVec4 vCleanColor{ 0, 0, 0, 0 };

	if (PeekMessageA(&Msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&Msg);
		DispatchMessageA(&Msg);
	}

	// Get the display size for centering elements
	ImGuiIO& io = ImGui::GetIO();
	settings::misc::center_x = io.DisplaySize.x / 2;
	settings::misc::center_y = io.DisplaySize.y / 2;

	// Start a new frame for ImGui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		// Execute the DrawLoop function provided by the user
		DrawLoop();

		// Toggle the menu visibility when the F2 key is pressed
		if (GetAsyncKeyState(VK_F2) & 1)
			settings::misc::show_menu = !settings::misc::show_menu;

		// If the menu is shown, make the overlay window clickable
		if (settings::misc::show_menu) {
			if (clickable != true)
				change_click(true);

			// Draw the menu content
			draw_menu();
		}
		// If the menu is hidden, make the overlay window not clickable
		else {
			if (clickable != false)
				change_click(false);
		}
	}

	// Render ImGui draw data and present the swap chain
	ImGui::Render();
	ImGui::EndFrame();
	PDeviceContext->ClearRenderTargetView(pRenderTarget, (float*)&vCleanColor);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	pSwapChain->Present(settings::misc::vsync, 0);

	// Return whether the message is not WM_QUIT, indicating the loop should continue
	return Msg.message != WM_QUIT;
}