#include "Utility.h"

#include <algorithm>
#include <codecvt>

#include <fstream>
#include <ScreenGrab.h>
#include <Shlwapi.h>

#include <managers/ShaderResourcesManager.h>
#include <rendering/models/Mesh.h>
#include <utils/Assert.h>

using namespace DirectX;

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

		void CalculateTangentArray(Mesh& mesh, std::vector<XMFLOAT3>& tangents)
		{
			const size_t vertexCount = mesh.Vertices().size();
			const size_t triangleCount = mesh.FaceCount();
			const std::vector<XMFLOAT3>& texCoords = mesh.TextureCoordinates();
			tangents.resize(vertexCount);

			XMFLOAT3 *tan1 = new XMFLOAT3[vertexCount * 2];
			XMFLOAT3 *tan2 = tan1 + vertexCount;
			ZeroMemory(tan1, vertexCount * sizeof(XMFLOAT3) * 2);

			size_t baseIndex = 0;
			for (long a = 0; a < triangleCount; a++)
			{
				long i1 = mesh.Indices()[baseIndex];
				long i2 = mesh.Indices()[baseIndex + 1];
				long i3 = mesh.Indices()[baseIndex + 2];

				const XMFLOAT3& v1 = mesh.Vertices()[i1];
				const XMFLOAT3& v2 = mesh.Vertices()[i2];
				const XMFLOAT3& v3 = mesh.Vertices()[i3];

				const XMFLOAT3& w1 = texCoords[i1];
				const XMFLOAT3& w2 = texCoords[i2];
				const XMFLOAT3& w3 = texCoords[i3];

				float x1 = v2.x - v1.x;
				float x2 = v3.x - v1.x;
				float y1 = v2.y - v1.y;
				float y2 = v3.y - v1.y;
				float z1 = v2.z - v1.z;
				float z2 = v3.z - v1.z;

				float s1 = w2.x - w1.x;
				float s2 = w3.x - w1.x;
				float t1 = w2.y - w1.y;
				float t2 = w3.y - w1.y;

				float r = 1.0F / (s1 * t2 - s2 * t1);
				XMFLOAT3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
				XMFLOAT3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

				XMStoreFloat3(&tan1[i1], XMVectorAdd(XMLoadFloat3(&tan1[i1]), XMLoadFloat3(&sdir)));
				XMStoreFloat3(&tan1[i2], XMVectorAdd(XMLoadFloat3(&tan1[i2]), XMLoadFloat3(&sdir)));
				XMStoreFloat3(&tan1[i3], XMVectorAdd(XMLoadFloat3(&tan1[i3]), XMLoadFloat3(&sdir)));

				XMStoreFloat3(&tan2[i1], XMVectorAdd(XMLoadFloat3(&tan2[i1]), XMLoadFloat3(&tdir)));
				XMStoreFloat3(&tan2[i2], XMVectorAdd(XMLoadFloat3(&tan2[i2]), XMLoadFloat3(&tdir)));
				XMStoreFloat3(&tan2[i3], XMVectorAdd(XMLoadFloat3(&tan2[i3]), XMLoadFloat3(&tdir)));

				baseIndex += 3;
			}

			for (long a = 0; a < vertexCount; a++)
			{
				const XMFLOAT3& n = mesh.Normals()[a];
				const XMFLOAT3& t = tan1[a];

				// Gram-Schmidt orthogonalize
				XMStoreFloat3(&tangents[a], XMVectorSubtract(XMLoadFloat3(&t), XMLoadFloat3(&n)));
				const float dot = n.x * t.x + n.y * t.y + n.z * t.z;
				XMStoreFloat3(&tangents[a], XMVectorScale(XMLoadFloat3(&tangents[a]), dot));
				XMStoreFloat3(&tangents[a], XMVector3Length(XMLoadFloat3(&tangents[a])));
			}

			delete[] tan1;
		}

		void CreateInitializedBuffer(const size_t id, const void* data, const unsigned int dataSize, const D3D11_USAGE usage, const unsigned int bindFlags) {
			ASSERT_PTR(data);
			ASSERT_COND(dataSize > 0);
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
			bufferDesc.ByteWidth = dataSize;
			bufferDesc.Usage = usage;
			bufferDesc.BindFlags = bindFlags;
			D3D11_SUBRESOURCE_DATA subResourceData;
			ZeroMemory(&subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
			subResourceData.pSysMem = data;
			ShaderResourcesManager::gInstance->AddBuffer(id, bufferDesc, &subResourceData);
		}

		void CreateNonInitializedBuffer(const size_t id, const unsigned int dataSize, const D3D11_USAGE usage, const unsigned int bindFlags) {
			ASSERT_COND(dataSize > 0);
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
			bufferDesc.ByteWidth = dataSize;
			bufferDesc.Usage = usage;
			bufferDesc.BindFlags = bindFlags;
			ShaderResourcesManager::gInstance->AddBuffer(id, bufferDesc, nullptr);
		}
	}
}