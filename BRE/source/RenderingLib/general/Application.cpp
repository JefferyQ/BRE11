#include "Application.h"

#include <d3d11_1.h>
#include <dinput.h>
#include <sstream>

#include <general/Camera.h>
#include <general/Component.h>
#include <input/Keyboard.h>
#include <input/Mouse.h>
#include <managers/DrawManager.h>
#include <managers/MaterialManager.h>
#include <managers/ModelManager.h>
#include <managers/SettingsManager.h>
#include <managers/ShadersManager.h>
#include <managers/ShaderResourcesManager.h>
#include <rendering/GlobalResources.h>
#include <rendering/RenderStateHelper.h>
#include <utils/Memory.h>
#include <utils/Utility.h>

using namespace DirectX;

namespace {
	POINT CenterWindow(const int windowWidth, const int windowHeight) {
		const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		POINT center;
		center.x = (screenWidth - windowWidth) / 2;
		center.y = (screenHeight - windowHeight) / 2;
		return center;
	}

	LRESULT WndProc(const HWND windowHandle, const unsigned int message, const WPARAM wParam, const LPARAM lParam) {
		switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(windowHandle, message, wParam, lParam);
	}

	typedef LRESULT(*WindowFun)(const HWND windowHandle, const unsigned int message, const WPARAM wParam, const LPARAM lParam);
	void InitWindow(const HINSTANCE& instance, const int showCommand, const unsigned screenWidth, const unsigned screenHeight, WNDCLASSEX& windowClass, HWND& windowHandle, WindowFun windowFun) {
		ZeroMemory(&windowClass, sizeof(windowClass));
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_CLASSDC;
		windowClass.lpfnWndProc = windowFun;
		windowClass.hInstance = instance;
		windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		windowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
		windowClass.lpszClassName = L"BRE";

		RECT windowRectangle = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };
		AdjustWindowRect(&windowRectangle, WS_POPUP, FALSE);

		RegisterClassEx(&windowClass);
		const POINT center = CenterWindow(screenWidth, screenHeight);
		const unsigned int windowWidth = windowRectangle.right - windowRectangle.left;
		const unsigned int windowHeight = windowRectangle.bottom - windowRectangle.top;
		windowHandle = CreateWindow(L"BRE", L"BRE", WS_POPUP, center.x, center.y, windowWidth, windowHeight, nullptr, nullptr, instance, nullptr);

		ShowWindow(windowHandle, showCommand);
		UpdateWindow(windowHandle);
	}

	void InitDirectX(
		const unsigned multisamplingCount,
		const unsigned screenWidth,
		const unsigned screenHeight,
		const unsigned frameRate,
		const HWND windowHandle,
		ID3D11Device1* &device,
		ID3D11DeviceContext1* &context,
		IDXGISwapChain1* & swapChain,
		ID3D11RenderTargetView* &backBufferRTV,
		ID3D11DepthStencilView* &primaryDSV) {
		unsigned int multisamplingQualityLevels;

		// Create device and device context
		{
			unsigned int createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

			D3D_FEATURE_LEVEL featureLevels[] = {
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0
			};

			ID3D11Device* direct3DDevice = nullptr;
			ID3D11DeviceContext* direct3DDeviceContext = nullptr;
			ASSERT_HR(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &direct3DDevice, nullptr, &direct3DDeviceContext));
			ASSERT_HR(direct3DDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&device)));
			ASSERT_HR(direct3DDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&context)));
			RELEASE_OBJECT(direct3DDevice);
			RELEASE_OBJECT(direct3DDeviceContext);

			device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, multisamplingCount, &multisamplingQualityLevels);
			ASSERT_COND(multisamplingQualityLevels != 0);
		}

		// Create swap chain
		{
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			swapChainDesc.Width = screenWidth;
			swapChainDesc.Height = screenHeight;
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.SampleDesc.Count = multisamplingCount;
			swapChainDesc.SampleDesc.Quality = multisamplingQualityLevels - 1;

			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 1;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			IDXGIDevice* dxgiDevice = nullptr;
			ASSERT_HR(device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)));

			IDXGIAdapter *dxgiAdapter = nullptr;
			ASSERT_HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter)));

			IDXGIFactory2* dxgiFactory = nullptr;
			ASSERT_HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory)));

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc;
			ZeroMemory(&fullScreenDesc, sizeof(fullScreenDesc));
			fullScreenDesc.RefreshRate.Numerator = frameRate;
			fullScreenDesc.RefreshRate.Denominator = 1;
			fullScreenDesc.Windowed = true;

			ASSERT_HR(dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, windowHandle, &swapChainDesc, &fullScreenDesc, nullptr, &swapChain));

			RELEASE_OBJECT(dxgiDevice);
			RELEASE_OBJECT(dxgiAdapter);
			RELEASE_OBJECT(dxgiFactory);
		}

		BRE::ShaderResourcesManager::gInstance = new BRE::ShaderResourcesManager(*device);

		// Create/Set render/depth stencil target view
		{
			ID3D11Texture2D* backBuffer;
			ASSERT_HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
			ASSERT_HR(device->CreateRenderTargetView(backBuffer, nullptr, &backBufferRTV));
			RELEASE_OBJECT(backBuffer);

			D3D11_TEXTURE2D_DESC depthStencilDesc;
			ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
			depthStencilDesc.Width = screenWidth;
			depthStencilDesc.Height = screenHeight;
			depthStencilDesc.MipLevels = 1;
			depthStencilDesc.ArraySize = 1;
			depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
			depthStencilDesc.SampleDesc.Count = multisamplingCount;
			depthStencilDesc.SampleDesc.Quality = multisamplingQualityLevels - 1;

			ID3D11Texture2D* depthStencilBuffer;
			BRE::ShaderResourcesManager::gInstance->AddTexture2D("primary_depth_stencil_texture2d", depthStencilDesc, nullptr, &depthStencilBuffer);
			ASSERT_PTR(depthStencilBuffer);
			BRE::ShaderResourcesManager::gInstance->AddDepthStencilView("primary_depth_stencil_texture2d", *depthStencilBuffer, nullptr, &primaryDSV);
			ASSERT_PTR(primaryDSV);
		}

		// Set viewport
		{
			D3D11_VIEWPORT viewport;
			ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;
			viewport.Width = static_cast<float>(screenWidth);
			viewport.Height = static_cast<float>(screenHeight);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			context->RSSetViewports(1, &viewport);
		}
	}
}

namespace BRE {
	Application::Application(const HINSTANCE& instance, const int showCommand) {
		srand(static_cast<unsigned int>(time(reinterpret_cast<time_t*>(0))));

		const char* appConfigFile = "content/configs/application.yml";
		SettingsManager::gInstance = new SettingsManager();
		SettingsManager::gInstance->LoadFile(appConfigFile);
		mScreenWidth = SettingsManager::gInstance->GetScalarAttribute<unsigned int>(appConfigFile, "application", "screenWidth");
		mScreenHeight = SettingsManager::gInstance->GetScalarAttribute<unsigned int>(appConfigFile, "application", "screenHeight");

		InitWindow(instance, showCommand, mScreenWidth, mScreenHeight, mWindowClass, mWindowHandle, WndProc);

		const unsigned multisamplingCount = SettingsManager::gInstance->GetScalarAttribute<unsigned int>(appConfigFile, "application", "multiSamplingCount");
		const unsigned int frameRate = SettingsManager::gInstance->GetScalarAttribute<unsigned int>(appConfigFile, "application", "frameRate");
		InitDirectX(multisamplingCount, mScreenWidth, mScreenHeight, frameRate, mWindowHandle, mDevice, mContext, mSwapChain, mBackBufferRTV, mPrimaryDSV);

		ShadersManager::gInstance = new ShadersManager(*mDevice);
		MaterialManager::gInstance = new MaterialManager();
		ModelManager::gInstance = new ModelManager();
		DrawManager::gInstance = new DrawManager(*mDevice, *mContext, mScreenWidth, mScreenHeight);
		RenderStateHelper::gInstance = new RenderStateHelper(*mContext);

		LPDIRECTINPUT8 directInput;
		ASSERT_HR(DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&directInput, nullptr));
		Keyboard::gInstance = new BRE::Keyboard(*directInput, mWindowHandle);
		Mouse::gInstance = new BRE::Mouse(*directInput, mWindowHandle);

		GlobalResources::gInstance = new GlobalResources();

		// Init camera
		const char* category = "camera";
		const std::vector<float> position = SettingsManager::gInstance->GetSequenceAttribute<float>(appConfigFile, category, "translation");
		ASSERT_COND(position.size() == 3);
		const XMFLOAT3 pos(position[0], position[1], position[2]);
		const std::vector<float> rotation = SettingsManager::gInstance->GetSequenceAttribute<float>(appConfigFile, category, "rotation");
		ASSERT_COND(rotation.size() == 3);
		const XMFLOAT3 rot(rotation[0], rotation[1], rotation[2]);
		const float fieldOfView = SettingsManager::gInstance->GetScalarAttribute<float>(appConfigFile, category, "fieldOfView");
		const float nearPlaneDistance = SettingsManager::gInstance->GetScalarAttribute<float>(appConfigFile, category, "nearPlaneDistance");
		const float farPlaneDistance = SettingsManager::gInstance->GetScalarAttribute<float>(appConfigFile, category, "farPlaneDistance");
		const float mouseSensibility = SettingsManager::gInstance->GetScalarAttribute<float>(appConfigFile, category, "mouseSensibility");
		const float rotationRate = SettingsManager::gInstance->GetScalarAttribute<float>(appConfigFile, category, "rotationRate");
		const float movementRate = SettingsManager::gInstance->GetScalarAttribute<float>(appConfigFile, category, "movementRate");
		const float aspectRatio = static_cast<float> (mScreenWidth) / mScreenHeight;
		Camera::gInstance = new BRE::Camera(pos, rot, fieldOfView, nearPlaneDistance, farPlaneDistance, aspectRatio, mouseSensibility, rotationRate, movementRate);
	}

	Application::~Application() {
		for (Component* component : mComponents) {
			DELETE_OBJECT(component);
		}
		delete SettingsManager::gInstance;
		delete ShaderResourcesManager::gInstance;
		delete ShadersManager::gInstance;
		delete DrawManager::gInstance;
		delete RenderStateHelper::gInstance;
		delete Keyboard::gInstance;
		delete Mouse::gInstance;
		delete GlobalResources::gInstance;
		delete Camera::gInstance;
		mContext->ClearState();
		UnregisterClass(L"BRE", mWindowClass.hInstance);
	}

	void Application::Run() {
		MSG message;
		ZeroMemory(&message, sizeof(message));
		mClock.Reset();
		while (message.message != WM_QUIT) {
			if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			else {
				mClock.UpdateTime();
				Update();
			}
		}
	}

	void Application::Update() {
		if (BRE::Keyboard::gInstance->WasKeyPressedThisFrame(DIK_ESCAPE)) {
			PostQuitMessage(0);
		}
		Keyboard::gInstance->Update();
		Mouse::gInstance->Update();
		const float elapsedTime = mClock.ElapsedTime();
		Camera::gInstance->Update(elapsedTime);
		for (Component* component : mComponents) {
			ASSERT_PTR(component);
			component->Update(elapsedTime);
		}
		std::wostringstream frameRate;
		frameRate << mClock.FrameRate();
		DrawManager::gInstance->FrameRateDrawer().Text() = frameRate.str();
		DrawManager::gInstance->DrawAll(*mDevice, *mContext, *mSwapChain, *mBackBufferRTV, *mPrimaryDSV);
	}
}