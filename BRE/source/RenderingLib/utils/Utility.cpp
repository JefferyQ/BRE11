#include "Utility.h"

#include <algorithm>
#include <codecvt>

#include <d3d11_1.h>
#include <fstream>
#include <ScreenGrab.h>
#include <Shlwapi.h>

#include <utils/Assert.h>

namespace BRE {
	namespace Utility {
		void GetFileName(const std::string& inputPath, std::string& filename) {
			std::string fullPath(inputPath);
			std::replace(fullPath.begin(), fullPath.end(), '\\', '/');

			const std::string::size_type lastSlashIndex = fullPath.find_last_of('/');

			if (lastSlashIndex == std::string::npos) {
				filename = fullPath;
			}
			else {
				filename = fullPath.substr(lastSlashIndex + 1, fullPath.size() - lastSlashIndex - 1);
			}
		}

		void GetDirectory(const std::string& inputPath, std::string& directory) {
			std::string fullPath(inputPath);
			std::replace(fullPath.begin(), fullPath.end(), '\\', '/');

			const std::string::size_type lastSlashIndex = fullPath.find_last_of('/');

			if (lastSlashIndex == std::string::npos) {
				directory = "";
			}
			else {
				directory = fullPath.substr(0, lastSlashIndex);
			}
		}

		void GetFileNameAndDirectory(const std::string& inputPath, std::string& directory, std::string& filename) {
			std::string fullPath(inputPath);
			std::replace(fullPath.begin(), fullPath.end(), '\\', '/');

			const std::string::size_type lastSlashIndex = fullPath.find_last_of('/');

			if (lastSlashIndex == std::string::npos) {
				directory = "";
				filename = fullPath;
			}
			else {
				directory = fullPath.substr(0, lastSlashIndex);
				filename = fullPath.substr(lastSlashIndex + 1, fullPath.size() - lastSlashIndex - 1);
			}
		}

		void LoadBinaryFile(const std::string& filename, std::vector<char>& data) {
			std::ifstream file(filename.c_str(), std::ios::binary);
			ASSERT_COND(file.bad() == false);

			file.seekg(0, std::ios::end);
			const unsigned int size = static_cast<unsigned int>(file.tellg());

			if (size > 0) {
				data.resize(size);
				file.seekg(0, std::ios::beg);
				file.read(&data.front(), size);
			}

			file.close();
		}

		void ToWideString(const std::string& source, std::wstring& dest) {
			dest.assign(source.begin(), source.end());
		}

		std::wstring ToWideString(const std::string& source) {
			std::wstring dest;
			dest.assign(source.begin(), source.end());

			return dest;
		}

		std::string ToString(const std::wstring& source) {
			//setup converter
			typedef std::codecvt_utf8<wchar_t> convert_type;
			std::wstring_convert<convert_type, wchar_t> converter;

			//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
			return converter.to_bytes(source);
		}

		void PathJoin(std::string& dest, const std::string& sourceDirectory, const std::string& sourceFile) {
			WCHAR buffer[MAX_PATH];
			const std::wstring wSourceDirectory = ToWideString(sourceDirectory);
			const std::wstring wSourceFile = ToWideString(sourceFile);
			PathCombine(buffer, wSourceDirectory.c_str(), wSourceFile.c_str());
			dest = ToString(std::wstring(buffer));
		}

		void GetPathExtension(const std::string& source, std::string& dest) {
			const std::wstring wSource = ToWideString(source);
			dest = ToString(PathFindExtension(wSource.c_str()));
		}

		void CopyData(ID3D11Device1& device, const void* data, const size_t sizeData, ID3D11Buffer& buffer) {
			ID3D11DeviceContext* context;
			device.GetImmediateContext(&context);
			ASSERT_PTR(context);

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
			context->Map(&buffer, 0, mapType, 0, &mappedResource);
			CopyMemory(mappedResource.pData, data, sizeData);
			context->Unmap(&buffer, 0);
		}

		float RandomFloat(const float min, const float max) {
			ASSERT_COND(max > min);			
			const float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			// generate (in your case) a float between 0 and (4.5-.78)
			// then add .78, giving you a float between .78 and 4.5
			const float range = max - min;
			return (random * range) + min;
		}

		size_t Hash(const char* str) {
			ASSERT_PTR(str);

			size_t hashValue = 0;
			while (*str) {
				hashValue = hashValue * 101 + static_cast<size_t> (*str++);
			}

			return hashValue;
		}

		void SaveTextureToFile(ID3D11DeviceContext1& device, ID3D11Texture2D* texture, const wchar_t* destFilename) {
			ASSERT_PTR(texture);
			ASSERT_PTR(destFilename);

			ASSERT_HR(DirectX::SaveDDSTextureToFile(&device, texture, destFilename));
		}
	}
}