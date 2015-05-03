#include "ShadersManager.h"

#include <d3d11_1.h>
#include <fstream>

#include <utils/Assert.h>
#include <utils/Memory.h>
#include <utils/Utility.h>

namespace BRE {
	ShadersManager* ShadersManager::gInstance = nullptr;

	ShadersManager::ShadersManager(ID3D11Device1& device)
		: mDevice(device)
	{
	}

	ShadersManager::~ShadersManager() {
		for (auto& elem : mInputLayouts) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mVertexShaders) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mPixelShaders) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mGeometryShaders) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mComputeShaders) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mHullShaders) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mDomainShaders) {
			RELEASE_OBJECT(elem.second);
		}
	}

	ID3D11VertexShader*	ShadersManager::LoadVertexShader(const char* filepath, const char* inputLayoutName, const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, const unsigned int* descNumElems) {
		ASSERT_PTR(filepath);
		const bool createInputLayout = inputLayoutName != nullptr && inputLayoutDesc != nullptr && descNumElems != nullptr;
		ASSERT_COND(!createInputLayout || (createInputLayout && mInputLayouts.find(*inputLayoutName) == mInputLayouts.end()));
		const size_t fileHash = Utility::Hash(filepath);
		VertexShaders::const_iterator findIt = mVertexShaders.find(fileHash);
		if (findIt != mVertexShaders.end()) {
			return findIt->second;
		}
		std::vector<char> shaderByteCode;
		StoreShaderByteCode(filepath, shaderByteCode);
		if (createInputLayout) {
			ID3D11InputLayout* inputLayout;
			BuildVertexLayout(shaderByteCode, inputLayoutDesc, *descNumElems, inputLayout);
			const size_t inputLayoutNameHash = Utility::Hash(inputLayoutName);
			mInputLayouts[inputLayoutNameHash] = inputLayout;
		}
		ID3D11VertexShader* shader = nullptr;
		ASSERT_HR(mDevice.CreateVertexShader(&shaderByteCode[0], shaderByteCode.size(), nullptr, &shader));
		mVertexShaders[fileHash] = shader;
		return shader;
	}

	ID3D11PixelShader* ShadersManager::LoadPixelShader(const char* filepath) {
		ASSERT_PTR(filepath);
		const size_t fileHash = Utility::Hash(filepath);
		PixelShaders::const_iterator findIt = mPixelShaders.find(fileHash);
		if (findIt != mPixelShaders.end()) {
			return findIt->second;
		}
		std::vector<char> shaderByteCode;
		StoreShaderByteCode(filepath, shaderByteCode);
		ID3D11PixelShader* shader = nullptr;
		ASSERT_HR(mDevice.CreatePixelShader(&shaderByteCode[0], shaderByteCode.size(), nullptr, &shader));
		mPixelShaders[fileHash] = shader;
		return shader;
	}

	ID3D11HullShader* ShadersManager::LoadHullShader(const char* filepath) {
		ASSERT_PTR(filepath);
		const size_t fileHash = Utility::Hash(filepath);
		HullShaders::const_iterator findIt = mHullShaders.find(fileHash);
		if (findIt != mHullShaders.end()) {
			return findIt->second;
		}
		std::vector<char> shaderByteCode;
		StoreShaderByteCode(filepath, shaderByteCode);
		ID3D11HullShader* shader = nullptr;
		ASSERT_HR(mDevice.CreateHullShader(&shaderByteCode[0], shaderByteCode.size(), nullptr, &shader));
		mHullShaders[fileHash] = shader;
		return shader;
	}

	ID3D11DomainShader* ShadersManager::LoadDomainShader(const char* filepath) {
		ASSERT_PTR(filepath);
		const size_t fileHash = Utility::Hash(filepath);
		DomainShaders::const_iterator findIt = mDomainShaders.find(fileHash);
		if (findIt != mDomainShaders.end()) {
			return findIt->second;
		}
		std::vector<char> shaderByteCode;
		StoreShaderByteCode(filepath, shaderByteCode);
		ID3D11DomainShader* shader = nullptr;
		ASSERT_HR(mDevice.CreateDomainShader(&shaderByteCode[0], shaderByteCode.size(), nullptr, &shader));
		mDomainShaders[fileHash] = shader;
		return shader;
	}

	ID3D11GeometryShader* ShadersManager::LoadGeometryShader(const char* filepath) {
		ASSERT_PTR(filepath);
		const size_t fileHash = Utility::Hash(filepath);
		GeometryShaders::const_iterator findIt = mGeometryShaders.find(fileHash);
		if (findIt != mGeometryShaders.end()) {
			return findIt->second;
		}
		std::vector<char> shaderByteCode;
		StoreShaderByteCode(filepath, shaderByteCode);
		ID3D11GeometryShader* shader = nullptr;
		ASSERT_HR(mDevice.CreateGeometryShader(&shaderByteCode[0], shaderByteCode.size(), nullptr, &shader));
		mGeometryShaders[fileHash] = shader;
		return shader;
	}

	ID3D11ComputeShader* ShadersManager::LoadComputeShader(const char* filepath) {
		ASSERT_PTR(filepath);
		const size_t fileHash = Utility::Hash(filepath);
		ComputeShaders::const_iterator findIt = mComputeShaders.find(fileHash);
		if (findIt != mComputeShaders.end()) {
			return findIt->second;
		}
		std::vector<char> shaderByteCode;
		StoreShaderByteCode(filepath, shaderByteCode);
		ID3D11ComputeShader* shader = nullptr;
		ASSERT_HR(mDevice.CreateComputeShader(&shaderByteCode[0], shaderByteCode.size(), nullptr, &shader));
		mComputeShaders[fileHash] = shader;
		return shader;
	}

	ID3D11InputLayout* ShadersManager::InputLayout(const char* id) const {
		const size_t idHash = Utility::Hash(id);
		InputLayouts::const_iterator findIt = mInputLayouts.find(idHash);
		ASSERT_COND(findIt != mInputLayouts.end());
		return findIt->second;
	}

	ID3D11InputLayout* ShadersManager::InputLayout(const size_t id) const {
		InputLayouts::const_iterator findIt = mInputLayouts.find(id);
		ASSERT_COND(findIt != mInputLayouts.end());
		return findIt->second;
	}

	void ShadersManager::StoreShaderByteCode(const char* fileName, std::vector<char>& buffer) const {
		ASSERT_COND(fileName);
		std::ifstream file;
		file.open(fileName, std::ios::binary);
		ASSERT_COND(file.is_open());
		file.seekg(0, std::ios_base::end);
		size_t size = static_cast<size_t> (file.tellg());
		file.seekg(0, std::ios_base::beg);
		buffer.resize(size);
		file.read(&buffer[0], size);
		file.close();
	}

	void ShadersManager::BuildVertexLayout(std::vector<char>& shaderByteCode, const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, const unsigned int inputLayoutDescSize, ID3D11InputLayout* &inputLayout) const {
		ASSERT_COND(!shaderByteCode.empty());
		ASSERT_HR(mDevice.CreateInputLayout(inputLayoutDesc, inputLayoutDescSize, &shaderByteCode[0], shaderByteCode.size(), &inputLayout));
	}
}