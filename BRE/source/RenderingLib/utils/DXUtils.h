#pragma once

#include <d3d11_1.h>

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11Texture2D;

namespace BRE {
	namespace Utils {
		void CopyData(ID3D11Device1& device, const void* data, const size_t sizeData, ID3D11Buffer& buffer);
		void SaveTextureToFile(ID3D11DeviceContext1& device, ID3D11Texture2D* texture, const wchar_t* destFilename);
		void CreateInitializedBuffer(const char* id, const void* data, const unsigned int dataSize, const D3D11_USAGE usage, const unsigned int bindFlags, ID3D11Buffer* *buffer = nullptr);
		void CreateNonInitializedBuffer(const char* id, const unsigned int dataSize, const D3D11_USAGE usage, const unsigned int bindFlags, ID3D11Buffer* *buffer = nullptr);
		void CreateDeviceAndContext(ID3D11Device1* &device, ID3D11DeviceContext1* &context, const unsigned int sampleCount, unsigned int& qualityLevels);
		void CreateSwapChain(ID3D11Device1& device, const unsigned int screenWidth, const unsigned int screenHeight, const unsigned int sampleCount, const unsigned int qualityLevels, const unsigned int frameRate, const HWND windowHandle, IDXGISwapChain1* &swapChain);
	}
}