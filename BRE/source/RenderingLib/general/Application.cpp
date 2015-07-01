#include "Application.h"

#include <d3d11_1.h>
#include <dinput.h>
#include <sstream>
#include <yaml-cpp/yaml.h>

#include <general/Camera.h>
#include <general/Component.h>
#include <input/Keyboard.h>
#include <input/Mouse.h>
#include <managers/DrawManager.h>
#include <managers/MaterialManager.h>
#include <managers/ModelManager.h>
#include <managers/ShadersManager.h>
#include <managers/ShaderResourcesManager.h>
#include <rendering/GlobalResources.h>
#include <rendering/RenderStateHelper.h>
#include <utils/DXUtils.h>
#include <utils/YamlUtils.h>

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
		const unsigned sampleCount,
		const unsigned screenWidth,
		const unsigned screenHeight,
		const unsigned frameRate,
		const HWND windowHandle,
		ID3D11Device1* &device,
		ID3D11DeviceContext1* &context,
		IDXGISwapChain1* &swapChain,
		ID3D11RenderTargetView* &backBufferRTV,
		ID3D11DepthStencilView* &depthStencilView,
		ID3D11ShaderResourceView* &depthStencilSRV) {
		unsigned int qualityLevels;

		BRE::Utils::CreateDeviceAndContext(device, context, sampleCount, qualityLevels);
		BRE::Utils::CreateSwapChain(*device, screenWidth, screenHeight, sampleCount, qualityLevels, frameRate, windowHandle, swapChain);

		BRE::ShaderResourcesManager::gInstance = new BRE::ShaderResourcesManager(*device);

		// Create/Set render/depth stencil target view
		{
			ID3D11Texture2D* backBuffer;
			ASSERT_HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
			ASSERT_HR(device->CreateRenderTargetView(backBuffer, nullptr, &backBufferRTV));
			backBuffer->Release();

			D3D11_TEXTURE2D_DESC depthStencilDesc;
			ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
			depthStencilDesc.Width = screenWidth;
			depthStencilDesc.Height = screenHeight;
			depthStencilDesc.MipLevels = 1;
			depthStencilDesc.ArraySize = 1;
			depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
			depthStencilDesc.SampleDesc.Count = sampleCount;
			depthStencilDesc.SampleDesc.Quality = qualityLevels - 1;

			ID3D11Texture2D* depthStencilBuffer;
			BRE::ShaderResourcesManager::gInstance->AddTexture2D("depth_stencil_texture", depthStencilDesc, nullptr, &depthStencilBuffer);
			BRE_ASSERT(depthStencilBuffer);

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
			depthStencilViewDesc.Flags = 0;
			depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			BRE::ShaderResourcesManager::gInstance->AddDepthStencilView("depth_stencil_view", *depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
			BRE_ASSERT(depthStencilView);

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
			shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			BRE::ShaderResourcesManager::gInstance->AddResourceSRV("depth_stencil_shader_resource_view", *depthStencilBuffer, &shaderResourceViewDesc, &depthStencilSRV);
			BRE_ASSERT(depthStencilSRV);
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
		const YAML::Node yamlFile = YAML::LoadFile("content/configs/settings.yml");
		BRE_ASSERT(yamlFile.IsDefined());
		const YAML::Node settingsNode = yamlFile["settings"];
		BRE_ASSERT(settingsNode.IsDefined());
		BRE_ASSERT(settingsNode.IsMap());
			
		mScreenWidth = YamlUtils::GetScalar<unsigned int>(settingsNode, "screenWidth");
		mScreenHeight = YamlUtils::GetScalar<unsigned int>(settingsNode, "screenHeight");

		InitWindow(instance, showCommand, mScreenWidth, mScreenHeight, mWindowClass, mWindowHandle, WndProc);
		const unsigned multisamplingCount = YamlUtils::GetScalar<unsigned int>(settingsNode, "multiSamplingCount");
		const unsigned int frameRate = YamlUtils::GetScalar<unsigned int>(settingsNode, "frameRate");
		InitDirectX(multisamplingCount, mScreenWidth, mScreenHeight, frameRate, mWindowHandle, mDevice, mContext, mSwapChain, mBackBufferRTV, mDepthStencilView, mDepthStencilSRV);

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
		Camera::InputData camData;
		float sequence[3];
		YamlUtils::GetSequence<float>(settingsNode, "translation", sequence, ARRAYSIZE(sequence));
		camData.mPos = XMFLOAT3(sequence[0], sequence[1], sequence[2]);
		YamlUtils::GetSequence<float>(settingsNode, "rotation", sequence, ARRAYSIZE(sequence));
		camData.mRotation = XMFLOAT3(sequence[0], sequence[1], sequence[2]);
		camData.mFieldOfView = YamlUtils::GetScalar<float>(settingsNode, "fieldOfView");
		camData.mNearPlaneDistance = YamlUtils::GetScalar<float>(settingsNode, "nearPlaneDistance");
		camData.mFarPlaneDistance = YamlUtils::GetScalar<float>(settingsNode, "farPlaneDistance");
		camData.mMouseSensitivity = YamlUtils::GetScalar<float>(settingsNode, "mouseSensitivity");
		camData.mRotationRate = YamlUtils::GetScalar<float>(settingsNode, "rotationRate");
		camData.mMovementRate = YamlUtils::GetScalar<float>(settingsNode, "movementRate");
		camData.mAspectRatio = static_cast<float> (mScreenWidth) / mScreenHeight;
		Camera::gInstance = new BRE::Camera(camData);
	}

	Application::~Application() {
		for (Component* component : mComponents) {
			delete component;
		}
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
		const float elapsedTime = mClock.ElapsedTime();
		if (BRE::Keyboard::gInstance->WasKeyPressedThisFrame(DIK_ESCAPE)) {
			PostQuitMessage(0);
		}
		Keyboard::gInstance->Update();
		Mouse::gInstance->Update();		
		Camera::gInstance->Update(elapsedTime);
		for (Component* component : mComponents) {
			BRE_ASSERT(component);
			component->Update(elapsedTime);
		}
		std::wostringstream frameRate;
		frameRate << mClock.FrameRate();
		DrawManager::gInstance->FrameRateDrawer().Text() = frameRate.str();
		DrawManager::gInstance->DrawAll(*mDevice, *mContext, *mSwapChain, *mBackBufferRTV, *mDepthStencilView, *mDepthStencilSRV);
	}
}