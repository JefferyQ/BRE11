#pragma once

#include <vector>

#include <general/Clock.h>
#include <utils/Assert.h>

struct ID3D11DepthStencilView;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct IDXGISwapChain1;

namespace BRE {
	class Component;

	class Application {
	public:
		Application(const HINSTANCE& instance, const int showCommand);
		~Application();

		ID3D11Device1& Device() { return *mDevice; }
		ID3D11DeviceContext1& Context() { return *mContext; }

		void Add(Component* component) { ASSERT_PTR(component); mComponents.push_back(component); }
		void Run();

	private:
		void Update();
		WNDCLASSEX mWindowClass;
		HWND mWindowHandle;

		ID3D11Device1* mDevice;
		ID3D11DeviceContext1* mContext;
		IDXGISwapChain1* mSwapChain;

		unsigned int mScreenWidth;
		unsigned int mScreenHeight;

		ID3D11RenderTargetView* mBackBufferRTV;
		ID3D11DepthStencilView* mDepthStencilView;
		ID3D11ShaderResourceView* mDepthStencilSRV;

		Clock mClock;

		std::vector<Component*> mComponents;
	};
}