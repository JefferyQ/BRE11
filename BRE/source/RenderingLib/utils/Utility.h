#pragma once

//////////////////////////////////////////////////////////////////////////
//
// Bunch of useful functions
//
//////////////////////////////////////////////////////////////////////////

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11Texture2D;

namespace BRE {
	class Mesh;

	namespace Utility {
		void GetFileName(const std::string& inputPath, std::string& filename);
		void GetDirectory(const std::string& inputPath, std::string& directory);
		void GetFileNameAndDirectory(const std::string& inputPath, std::string& directory, std::string& filename);
		void LoadBinaryFile(const std::string& filename, std::vector<char>& data);
		void ToWideString(const std::string& source, std::wstring& dest);
		std::wstring ToWideString(const std::string& source);
		std::string ToString(const std::wstring& source);
		void PathJoin(std::string& dest, const std::string& sourceDirectory, const std::string& sourceFile);
		void GetPathExtension(const std::string& source, std::string& dest);

		// Copies the content from data to the buffer
		// that will be used by the GPU.
		// This call should be made as infrequently as possible.
		void CopyData(ID3D11Device1& device, const void* data, const size_t sizeData, ID3D11Buffer& buffer);

		float RandomFloat(const float min, const float max);

		size_t Hash(const char* str);

		void SaveTextureToFile(ID3D11DeviceContext1& device, ID3D11Texture2D* texture, const wchar_t* destFilename);

		void CalculateTangentArray(Mesh& mesh, std::vector<DirectX::XMFLOAT3>& tangents);

		void CreateInitializedBuffer(const char* id, const void* data, const unsigned int dataSize, const D3D11_USAGE usage, const unsigned int bindFlags, ID3D11Buffer* *buffer = nullptr);
		void CreateNonInitializedBuffer(const char* id, const unsigned int dataSize, const D3D11_USAGE usage, const unsigned int bindFlags, ID3D11Buffer* *buffer = nullptr);
	};
}