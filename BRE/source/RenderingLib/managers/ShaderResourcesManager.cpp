#include "ShaderResourcesManager.h"

#include <cassert>
#include <DDSTextureLoader.h>
#include <d3d11_1.h>
#include <fstream>
#include <vector>
#include <WICTextureLoader.h>

#include <utils/Assert.h>
#include <utils/Memory.h>
#include <utils/Utility.h>

namespace BRE {
	ShaderResourcesManager* ShaderResourcesManager::gInstance = nullptr;

	ShaderResourcesManager::~ShaderResourcesManager() {
		for (auto& elem : mShaderResourceViews) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mUnorderedAccessViews) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mBuffers) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mTextures2D) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mRasterizerStates) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mRenderTargetViews) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mDepthStencilViews) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mBlendStates) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mDepthStencilStates) {
			RELEASE_OBJECT(elem.second);
		}
		for (auto& elem : mSamplerStates) {
			RELEASE_OBJECT(elem.second);
		}
	}

	ID3D11ShaderResourceView* ShaderResourcesManager::AddTextureFromFileSRV(const char* filepath) {
		ASSERT_PTR(filepath);

		const size_t fileHash = Utility::Hash(filepath);

		// Names must be unique
		ShaderResourceViews::iterator findIt = mShaderResourceViews.find(fileHash);
		if (findIt != mShaderResourceViews.end()) {
			return findIt->second;
		}
		 
		ID3D11Resource* texture;
		ID3D11ShaderResourceView* elem;
		ASSERT_HR(DirectX::CreateDDSTextureFromFile(&mDevice, Utility::ToWideString(filepath).c_str(), &texture, &elem));

		texture->Release();

		mShaderResourceViews[fileHash] = elem;

		return elem;
	}

	ID3D11ShaderResourceView* ShaderResourcesManager::AddResourceSRV(const char* id, ID3D11Resource& resource, const D3D11_SHADER_RESOURCE_VIEW_DESC* desc) {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		// Names must be unique
		if (mShaderResourceViews.find(idHash) != mShaderResourceViews.end()) {
			return nullptr;
		}

		ID3D11ShaderResourceView* elem;
		ASSERT_HR(mDevice.CreateShaderResourceView(&resource, desc, &elem));

		mShaderResourceViews[idHash] = elem;

		return elem;
	}

	ID3D11ShaderResourceView* ShaderResourcesManager::AddResourceSRV(const size_t id, ID3D11Resource& resource, const D3D11_SHADER_RESOURCE_VIEW_DESC* desc) {
		// Names must be unique
		if (mShaderResourceViews.find(id) != mShaderResourceViews.end()) {
			return nullptr;
		}

		ID3D11ShaderResourceView* elem;
		ASSERT_HR(mDevice.CreateShaderResourceView(&resource, desc, &elem));

		mShaderResourceViews[id] = elem;

		return elem;
	}

	ID3D11UnorderedAccessView* ShaderResourcesManager::AddResourceUAV(const char* id, ID3D11Resource& resource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc) {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		// Names must be unique
		if (mUnorderedAccessViews.find(idHash) != mUnorderedAccessViews.end()) {
			return nullptr;
		}

		ID3D11UnorderedAccessView* elem;
		ASSERT_HR(mDevice.CreateUnorderedAccessView(&resource, &desc, &elem));

		mUnorderedAccessViews[idHash] = elem;

		return elem;
	}

	ID3D11UnorderedAccessView* ShaderResourcesManager::AddResourceUAV(const size_t id, ID3D11Resource& resource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc) {
		// Names must be unique
		if (mUnorderedAccessViews.find(id) != mUnorderedAccessViews.end()) {
			return nullptr;
		}

		ID3D11UnorderedAccessView* elem;
		ASSERT_HR(mDevice.CreateUnorderedAccessView(&resource, &desc, &elem));

		mUnorderedAccessViews[id] = elem;

		return elem;
	}

	ID3D11Buffer* ShaderResourcesManager::AddBuffer(const char* id, D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA* const initData) {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		// Names must be unique
		if (mBuffers.find(idHash) != mBuffers.end()) {
			return nullptr;
		}

		// If the bind flag is D3D11_BIND_CONSTANT_BUFFER,
		// you must set the ByteWidth value in multiples of 16,
		// and less than or equal to D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.
		if (desc.BindFlags == D3D11_BIND_CONSTANT_BUFFER) {
			const size_t baseAlignment = 16;
			desc.ByteWidth = static_cast<unsigned int>(desc.ByteWidth + (baseAlignment - (desc.ByteWidth % baseAlignment)));
		}

		ID3D11Buffer* buffer;
		ASSERT_HR(mDevice.CreateBuffer(&desc, initData, &buffer));

		mBuffers[idHash] = buffer;

		return buffer;
	}

	ID3D11Buffer* ShaderResourcesManager::AddBuffer(const size_t id, D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA* const initData) {
		// Names must be unique
		if (mBuffers.find(id) != mBuffers.end()) {
			return nullptr;
		}

		// If the bind flag is D3D11_BIND_CONSTANT_BUFFER,
		// you must set the ByteWidth value in multiples of 16,
		// and less than or equal to D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.
		if (desc.BindFlags == D3D11_BIND_CONSTANT_BUFFER) {
			const size_t baseAlignment = 16;
			desc.ByteWidth = static_cast<unsigned int>(desc.ByteWidth + (baseAlignment - (desc.ByteWidth % baseAlignment)));
		}

		ID3D11Buffer* buffer;
		ASSERT_HR(mDevice.CreateBuffer(&desc, initData, &buffer));

		mBuffers[id] = buffer;

		return buffer;
	}

	ID3D11Texture2D* ShaderResourcesManager::AddTexture2D(const char* id, const D3D11_TEXTURE2D_DESC& texDesc, const D3D11_SUBRESOURCE_DATA* initialData) {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		// Names must be unique
		if (mTextures2D.find(idHash) != mTextures2D.end()) {
			return nullptr;
		}

		ID3D11Texture2D* elem;
		ASSERT_HR(mDevice.CreateTexture2D(&texDesc, initialData, &elem));

		mTextures2D[idHash] = elem;

		return elem;
	}

	ID3D11Texture2D* ShaderResourcesManager::AddTexture2D(const size_t id, const D3D11_TEXTURE2D_DESC& texDesc, const D3D11_SUBRESOURCE_DATA* initialData) {
		// Names must be unique
		if (mTextures2D.find(id) != mTextures2D.end()) {
			return nullptr;
		}

		ID3D11Texture2D* elem;
		ASSERT_HR(mDevice.CreateTexture2D(&texDesc, initialData, &elem));

		mTextures2D[id] = elem;

		return elem;
	}

	ID3D11RasterizerState* ShaderResourcesManager::AddRasterizerState(const char* id, const D3D11_RASTERIZER_DESC& desc) {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		// Names must be unique
		if (mRasterizerStates.find(idHash) != mRasterizerStates.end()) {
			return nullptr;
		}

		ID3D11RasterizerState* elem;
		ASSERT_HR(mDevice.CreateRasterizerState(&desc, &elem));

		mRasterizerStates[idHash] = elem;

		return elem;
	}

	ID3D11RasterizerState* ShaderResourcesManager::AddRasterizerState(const size_t id, const D3D11_RASTERIZER_DESC& desc) {
		// Names must be unique
		if (mRasterizerStates.find(id) != mRasterizerStates.end()) {
			return nullptr;
		}

		ID3D11RasterizerState* elem;
		ASSERT_HR(mDevice.CreateRasterizerState(&desc, &elem));

		mRasterizerStates[id] = elem;

		return elem;
	}

	ID3D11RenderTargetView* ShaderResourcesManager::AddRenderTargetView(const char* id, ID3D11Resource& resource, const D3D11_RENDER_TARGET_VIEW_DESC* desc) {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		// Names must be unique
		if (mRenderTargetViews.find(idHash) != mRenderTargetViews.end()) {
			return nullptr;
		}

		ID3D11RenderTargetView* elem;
		ASSERT_HR(mDevice.CreateRenderTargetView(&resource, desc, &elem));

		mRenderTargetViews[idHash] = elem;

		return elem;
	}

	ID3D11RenderTargetView* ShaderResourcesManager::AddRenderTargetView(const size_t id, ID3D11Resource& resource, const D3D11_RENDER_TARGET_VIEW_DESC* desc) {
		// Names must be unique
		if (mRenderTargetViews.find(id) != mRenderTargetViews.end()) {
			return nullptr;
		}

		ID3D11RenderTargetView* elem;
		ASSERT_HR(mDevice.CreateRenderTargetView(&resource, desc, &elem));

		mRenderTargetViews[id] = elem;

		return elem;
	}

	ID3D11DepthStencilView* ShaderResourcesManager::AddDepthStencilView(const char* id, ID3D11Resource& resource, const D3D11_DEPTH_STENCIL_VIEW_DESC* desc) {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		// Names must be unique
		if (mDepthStencilViews.find(idHash) != mDepthStencilViews.end()) {
			return nullptr;
		}

		ID3D11DepthStencilView* elem;
		ASSERT_HR(mDevice.CreateDepthStencilView(&resource, desc, &elem));

		mDepthStencilViews[idHash] = elem;

		return elem;
	}

	ID3D11DepthStencilView* ShaderResourcesManager::AddDepthStencilView(const size_t id, ID3D11Resource& resource, const D3D11_DEPTH_STENCIL_VIEW_DESC* desc) {
		// Names must be unique
		if (mDepthStencilViews.find(id) != mDepthStencilViews.end()) {
			return nullptr;
		}

		ID3D11DepthStencilView* elem;
		ASSERT_HR(mDevice.CreateDepthStencilView(&resource, desc, &elem));

		mDepthStencilViews[id] = elem;

		return elem;
	}

	ID3D11BlendState* ShaderResourcesManager::AddBlendState(const char* id, const D3D11_BLEND_DESC& desc) {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		// Names must be unique
		if (mBlendStates.find(idHash) != mBlendStates.end()) {
			return nullptr;
		}

		ID3D11BlendState* elem;
		ASSERT_HR(mDevice.CreateBlendState(&desc, &elem));

		mBlendStates[idHash] = elem;

		return elem;
	}

	ID3D11BlendState* ShaderResourcesManager::AddBlendState(const size_t id, const D3D11_BLEND_DESC& desc) {
		// Names must be unique
		if (mBlendStates.find(id) != mBlendStates.end()) {
			return nullptr;
		}

		ID3D11BlendState* elem;
		ASSERT_HR(mDevice.CreateBlendState(&desc, &elem));

		mBlendStates[id] = elem;

		return elem;
	}

	ID3D11DepthStencilState* ShaderResourcesManager::AddDepthStencilState(const char* id, const D3D11_DEPTH_STENCIL_DESC& desc) {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		// Names must be unique
		if (mDepthStencilStates.find(idHash) != mDepthStencilStates.end()) {
			return nullptr;
		}

		ID3D11DepthStencilState* elem;
		ASSERT_HR(mDevice.CreateDepthStencilState(&desc, &elem));

		mDepthStencilStates[idHash] = elem;

		return elem;
	}

	ID3D11DepthStencilState* ShaderResourcesManager::AddDepthStencilState(const size_t id, const D3D11_DEPTH_STENCIL_DESC& desc) {
		// Names must be unique
		if (mDepthStencilStates.find(id) != mDepthStencilStates.end()) {
			return nullptr;
		}

		ID3D11DepthStencilState* elem;
		ASSERT_HR(mDevice.CreateDepthStencilState(&desc, &elem));

		mDepthStencilStates[id] = elem;

		return elem;
	}

	ID3D11SamplerState* ShaderResourcesManager::AddSamplerState(const char* id, const D3D11_SAMPLER_DESC& desc) {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		// Names must be unique
		if (mSamplerStates.find(idHash) != mSamplerStates.end()) {
			return nullptr;
		}

		ID3D11SamplerState* elem;
		ASSERT_HR(mDevice.CreateSamplerState(&desc, &elem));

		mSamplerStates[idHash] = elem;

		return elem;
	}

	ID3D11SamplerState* ShaderResourcesManager::AddSamplerState(const size_t id, const D3D11_SAMPLER_DESC& desc) {
		// Names must be unique
		if (mSamplerStates.find(id) != mSamplerStates.end()) {
			return nullptr;
		}

		ID3D11SamplerState* elem;
		ASSERT_HR(mDevice.CreateSamplerState(&desc, &elem));

		mSamplerStates[id] = elem;

		return elem;
	}

	ID3D11ShaderResourceView* ShaderResourcesManager::ShaderResourceView(const char* id) const {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		ShaderResourceViews::const_iterator it = mShaderResourceViews.find(idHash);
		if (it != mShaderResourceViews.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11ShaderResourceView* ShaderResourcesManager::ShaderResourceView(const size_t id) const {
		ShaderResourceViews::const_iterator it = mShaderResourceViews.find(id);
		if (it != mShaderResourceViews.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11UnorderedAccessView* ShaderResourcesManager::UnorderedAccessView(const char* id) const {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		UnorderedAccessViews::const_iterator it = mUnorderedAccessViews.find(idHash);
		if (it != mUnorderedAccessViews.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11UnorderedAccessView* ShaderResourcesManager::UnorderedAccessView(const size_t id) const {
		UnorderedAccessViews::const_iterator it = mUnorderedAccessViews.find(id);
		if (it != mUnorderedAccessViews.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11Buffer* ShaderResourcesManager::Buffer(const char* id) const {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		Buffers::const_iterator it = mBuffers.find(idHash);
		if (it != mBuffers.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11Buffer* ShaderResourcesManager::Buffer(const size_t id) const {
		Buffers::const_iterator it = mBuffers.find(id);
		if (it != mBuffers.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11Texture2D* ShaderResourcesManager::Texture2D(const char* id) const {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		Textures2D::const_iterator it = mTextures2D.find(idHash);
		if (it != mTextures2D.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11Texture2D* ShaderResourcesManager::Texture2D(const size_t id) const {
		Textures2D::const_iterator it = mTextures2D.find(id);
		if (it != mTextures2D.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11RasterizerState* ShaderResourcesManager::RasterizerState(const char* id) const {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		RasterizerStates::const_iterator it = mRasterizerStates.find(idHash);
		if (it != mRasterizerStates.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11RasterizerState* ShaderResourcesManager::RasterizerState(const size_t id) const {
		RasterizerStates::const_iterator it = mRasterizerStates.find(id);
		if (it != mRasterizerStates.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11RenderTargetView* ShaderResourcesManager::RenderTargetView(const char* id) const {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		RenderTargetViews::const_iterator it = mRenderTargetViews.find(idHash);
		if (it != mRenderTargetViews.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11RenderTargetView* ShaderResourcesManager::RenderTargetView(const size_t id) const {
		RenderTargetViews::const_iterator it = mRenderTargetViews.find(id);
		if (it != mRenderTargetViews.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11DepthStencilView* ShaderResourcesManager::DepthStencilView(const char* id) const {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		DepthStencilViews::const_iterator it = mDepthStencilViews.find(idHash);
		if (it != mDepthStencilViews.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11DepthStencilView* ShaderResourcesManager::DepthStencilView(const size_t id) const {
		DepthStencilViews::const_iterator it = mDepthStencilViews.find(id);
		if (it != mDepthStencilViews.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11BlendState* ShaderResourcesManager::BlendState(const char* id) const {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		BlendStates::const_iterator it = mBlendStates.find(idHash);
		if (it != mBlendStates.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11BlendState* ShaderResourcesManager::BlendState(const size_t id) const {
		BlendStates::const_iterator it = mBlendStates.find(id);
		if (it != mBlendStates.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11DepthStencilState* ShaderResourcesManager::DepthStencilState(const char* id) const {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		DepthStencilStates::const_iterator it = mDepthStencilStates.find(idHash);
		if (it != mDepthStencilStates.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11DepthStencilState* ShaderResourcesManager::DepthStencilState(const size_t id) const {
		DepthStencilStates::const_iterator it = mDepthStencilStates.find(id);
		if (it != mDepthStencilStates.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11SamplerState* ShaderResourcesManager::SamplerState(const char* id) const {
		ASSERT_PTR(id);

		const size_t idHash = Utility::Hash(id);

		SamplerStates::const_iterator it = mSamplerStates.find(idHash);
		if (it != mSamplerStates.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	ID3D11SamplerState* ShaderResourcesManager::SamplerState(const size_t id) const {
		SamplerStates::const_iterator it = mSamplerStates.find(id);
		if (it != mSamplerStates.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}
}