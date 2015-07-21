#include "DXUtils.h"

#include <ScreenGrab.h>

#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>

namespace BRE {
	namespace Utils {
		void CopyData(ID3D11Device1& device, const void* data, const size_t sizeData, ID3D11Buffer& buffer) {
			ID3D11DeviceContext* context;
			device.GetImmediateContext(&context);
			BRE_ASSERT(context);
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
			context->Map(&buffer, 0, mapType, 0, &mappedResource);
			CopyMemory(mappedResource.pData, data, sizeData);
			context->Unmap(&buffer, 0);
		}

		void SaveTextureToFile(ID3D11DeviceContext1& device, ID3D11Texture2D* texture, const wchar_t* destFilename) {
			BRE_ASSERT(texture);
			BRE_ASSERT(destFilename);
			ASSERT_HR(DirectX::SaveDDSTextureToFile(&device, texture, destFilename));
		}

		void CreateInitializedBuffer(const char* id, const void* data, const unsigned int dataSize, const D3D11_USAGE usage, const unsigned int bindFlags, ID3D11Buffer* *buffer) {
			BRE_ASSERT(data);
			BRE_ASSERT(dataSize > 0);
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
			bufferDesc.ByteWidth = dataSize;
			bufferDesc.Usage = usage;
			bufferDesc.BindFlags = bindFlags;
			D3D11_SUBRESOURCE_DATA subResourceData;
			ZeroMemory(&subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
			subResourceData.pSysMem = data;
			ShaderResourcesManager::gInstance->AddBuffer(id, bufferDesc, &subResourceData, buffer);
		}

		void CreateNonInitializedBuffer(const char* id, const unsigned int dataSize, const D3D11_USAGE usage, const unsigned int bindFlags, ID3D11Buffer* *buffer) {
			BRE_ASSERT(dataSize > 0);
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
			bufferDesc.ByteWidth = dataSize;
			bufferDesc.Usage = usage;
			bufferDesc.BindFlags = bindFlags;
			ShaderResourcesManager::gInstance->AddBuffer(id, bufferDesc, nullptr, buffer);
		}

		void CreateDeviceAndContext(ID3D11Device1* &device, ID3D11DeviceContext1* &context, const unsigned int sampleCount, unsigned int& qualityLevels) {
			unsigned int createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
			D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

			ID3D11Device* direct3DDevice = nullptr;
			ID3D11DeviceContext* direct3DDeviceContext = nullptr;
			ASSERT_HR(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &direct3DDevice, nullptr, &direct3DDeviceContext));
			ASSERT_HR(direct3DDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&device)));
			ASSERT_HR(direct3DDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&context)));
			direct3DDevice->Release();
			direct3DDeviceContext->Release();
			device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &qualityLevels);
			BRE_ASSERT(qualityLevels != 0);
		}

		void CreateSwapChain(ID3D11Device1& device, const unsigned int screenWidth, const unsigned int screenHeight, const unsigned int sampleCount, const unsigned int qualityLevels, const unsigned int frameRate, const HWND windowHandle, IDXGISwapChain1* &swapChain) {
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			swapChainDesc.Width = screenWidth;
			swapChainDesc.Height = screenHeight;
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.SampleDesc.Count = sampleCount;
			swapChainDesc.SampleDesc.Quality = qualityLevels - 1;

			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 1;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			IDXGIDevice* dxgiDevice = nullptr;
			ASSERT_HR(device.QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)));

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

			dxgiDevice->Release();
			dxgiAdapter->Release();
			dxgiFactory->Release();
		}
	}
}