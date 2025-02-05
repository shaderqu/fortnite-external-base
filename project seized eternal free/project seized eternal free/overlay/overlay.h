#pragma once
#include "../includes/includes.hpp"
#include <functional>


namespace Overlay
{
	class DX11Overlay
	{
	private:
		static WNDCLASSEX wcex;
		static MARGINS Margin;

		static ID3D11Device* pDevice;
		static ID3D11DeviceContext* PDeviceContext;
		static IDXGISwapChain* pSwapChain;
		static ID3D11RenderTargetView* pRenderTarget;

		static BOOL CreateSwapchain();
		static BOOL CreateOverlay();
		static BOOL CreateRenderTarget();
		static BOOL InitImGui();
		static VOID ChechWindow(HWND GameWindow);
		static BOOL Cleanup();
	public:
		static BOOL MainLoop(std::function<VOID()> DrawLoop);

		//static Initialize::STATUS InitOverlay();
		static BOOL InitOverlay();
		static VOID GetWindow(HWND GameWindow);
		//static BOOL IsKeyPressed(int VK_CODE, bool ShowMenu = false);

		static HINSTANCE InjectedModule;

		static RECT Rect;

		static HWND PSEFOverlay;

		static INT width;
		static INT height;
		static INT widthD2;
		static INT heightD2;
	};
}
