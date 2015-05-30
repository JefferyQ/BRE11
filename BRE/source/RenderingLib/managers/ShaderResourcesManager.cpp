#include "ShaderResourcesManager.h"

#include <cassert>
#include <DDSTextureLoader.h>
#include <d3d11_1.h>
#include <fstream>
#include <vector>
#include <WICTextureLoader.h>

#include <utils/Assert.h>
#include <utils/Memory.h>
#include <utils/Hash.h>
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

	size_t ShaderResourcesManager::AddTextureFromFileSRV(const char* filepath, ID3D11ShaderResourceView* *resource) {
		ASSERT_PTR(filepath);
		const size_t id = Hash(filepath);
		ShaderResourceViews::iterator findIt = mShaderResourceViews.find(id);
		if (findIt != mShaderResourceViews.end()) {
			if (resource) *resource = findIt->second;
			return id; 
		}

		ID3D11Resource* texture;
		ID3D11ShaderResourceView* elem;
		ASSERT_HR(DirectX::CreateDDSTextureFromFile(&mDevice, Utility::ToWideString(filepath).c_str(), &texture, &elem));
		texture->Release();
		mShaderResourceViews[id] = elem;
		if (resource) *resource = elem;
		return id;
	}

	size_t ShaderResourcesManager::AddResourceSRV(const char* id, ID3D11Resource& resource, const D3D11_SHADER_RESOURCE_VIEW_DESC* desc, ID3D11ShaderResourceView* *view) {
		ASSERT_PTR(id);
		const size_t idHash = Hash(id);
		ASSERT_COND(mShaderResourceViews.find(idHash) == mShaderResourceViews.end());
		ID3D11ShaderResourceView* elem;
		ASSERT_HR(mDevice.CreateShaderResourceView(&resource, desc, &elem));
		mShaderResourceViews[idHash] = elem;
		if (view) *view = elem;
		return idHash;
	}

	size_t ShaderResourcesManager::AddResourceUAV(const char* id, ID3D11Resource& resource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc, ID3D11UnorderedAccessView* *view) {
		ASSERT_PTR(id);
		const size_t idHash = Hash(id);
		ASSERT_COND(mUnorderedAccessViews.find(idHash) == mUnorderedAccessViews.end())
		ID3D11UnorderedAccessView* elem;
		ASSERT_HR(mDevice.CreateUnorderedAccessView(&resource, &desc, &elem));
		mUnorderedAccessViews[idHash] = elem;
		if (view) *view = elem;
		return idHash;
	}

	size_t ShaderResourcesManager::AddBuffer(const char* id, D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA* const initData, ID3D11Buffer* *buffer) {
		ASSERT_PTR(id);
		const size_t idHash = Hash(id);
		ASSERT_COND(mBuffers.find(idHash) == mBuffers.end());

		// If the bind flag is D3D11_BIND_CONSTANT_BUFFER,
		// you must set the ByteWidth value in multiples of 16,
		// and less than or equal to D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.
		if (desc.BindFlags == D3D11_BIND_CONSTANT_BUFFER) {
			const size_t baseAlignment = 16;
			desc.ByteWidth = static_cast<unsigned int>(desc.ByteWidth + (baseAlignment - (desc.ByteWidth % baseAlignment)));
		}
		ID3D11Buffer* elem;
		ASSERT_HR(mDevice.CreateBuffer(&desc, initData, &elem));
		mBuffers[idHash] = elem;
		if (buffer) *buffer = elem;
		return idHash;
	}

	size_t ShaderResourcesManager::AddTexture2D(const char* id, const D3D11_TEXTURE2D_DESC& texDesc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture2D* *texture) {
		ASSERT_PTR(id);
		const size_t idHash = Hash(id);
		ASSERT_COND(mTextures2D.find(idHash) == mTextures2D.end());
		ID3D11Texture2D* elem;
		ASSERT_HR(mDevice.CreateTexture2D(&texDesc, initialData, &elem));
		mTextures2D[idHash] = elem;
		if (texture) *texture = elem;
		return idHash;
	}

	size_t ShaderResourcesManager::AddRasterizerState(const char* id, const D3D11_RASTERIZER_DESC& desc, ID3D11RasterizerState* *state) {
		ASSERT_PTR(id);
		const size_t idHash = Hash(id);
		ASSERT_COND(mRasterizerStates.find(idHash) == mRasterizerStates.end());
		ID3D11RasterizerState* elem;
		ASSERT_HR(mDevice.CreateRasterizerState(&desc, &elem));
		mRasterizerStates[idHash] = elem;
		if (state) *state = elem;
		return idHash;
	}

	size_t ShaderResourcesManager::AddRenderTargetView(const char* id, ID3D11Resource& resource, const D3D11_RENDER_TARGET_VIEW_DESC* desc, ID3D11RenderTargetView* *view) {
		ASSERT_PTR(id);
		const size_t idHash = Hash(id);
		ASSERT_COND(mRenderTargetViews.find(idHash) == mRenderTargetViews.end());
		ID3D11RenderTargetView* elem;
		ASSERT_HR(mDevice.CreateRenderTargetView(&resource, desc, &elem));
		mRenderTargetViews[idHash] = elem;
		if (view) *view = elem;
		return idHash;
	}

	size_t ShaderResourcesManager::AddDepthStencilView(const char* id, ID3D11Resource& resource, const D3D11_DEPTH_STENCIL_VIEW_DESC* desc, ID3D11DepthStencilView* *view) {
		ASSERT_PTR(id);
		const size_t idHash = Hash(id);
		ASSERT_COND(mDepthStencilViews.find(idHash) == mDepthStencilViews.end());
		ID3D11DepthStencilView* elem;
		ASSERT_HR(mDevice.CreateDepthStencilView(&resource, desc, &elem));
		mDepthStencilViews[idHash] = elem;
		if (view) *view = elem;
		return idHash;
	}

	size_t ShaderResourcesManager::AddBlendState(const char* id, const D3D11_BLEND_DESC& desc, ID3D11BlendState* *state) {
		ASSERT_PTR(id);
		const size_t idHash = Hash(id);
		ASSERT_COND(mBlendStates.find(idHash) == mBlendStates.end());
		ID3D11BlendState* elem;
		ASSERT_HR(mDevice.CreateBlendState(&desc, &elem));
		mBlendStates[idHash] = elem;
		if (state) *state = elem;
		return idHash;
	}

	size_t ShaderResourcesManager::AddDepthStencilState(const char* id, const D3D11_DEPTH_STENCIL_DESC& desc, ID3D11DepthStencilState* *state) {
		ASSERT_PTR(id);
		const size_t idHash = Hash(id);
		ASSERT_COND(mDepthStencilStates.find(idHash) == mDepthStencilStates.end());
		ID3D11DepthStencilState* elem;
		ASSERT_HR(mDevice.CreateDepthStencilState(&desc, &elem));
		mDepthStencilStates[idHash] = elem;
		if (state) *state = elem;
		return idHash;
	}

	size_t ShaderResourcesManager::AddSamplerState(const char* id, const D3D11_SAMPLER_DESC& desc, ID3D11SamplerState* *state) {
		ASSERT_PTR(id);
		const size_t idHash = Hash(id);
		ASSERT_COND(mSamplerStates.find(idHash) == mSamplerStates.end());
		ID3D11SamplerState* elem;
		ASSERT_HR(mDevice.CreateSamplerState(&desc, &elem));
		mSamplerStates[idHash] = elem;
		if (state) *state = elem;
		return idHash;
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

	ID3D11UnorderedAccessView* ShaderResourcesManager::UnorderedAccessView(const size_t id) const {
		UnorderedAccessViews::const_iterator it = mUnorderedAccessViews.find(id);
		if (it != mUnorderedAccessViews.end()) {
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
	
	ID3D11Texture2D* ShaderResourcesManager::Texture2D(const size_t id) const {
		Textures2D::const_iterator it = mTextures2D.find(id);
		if (it != mTextures2D.end()) {
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
	
	ID3D11RenderTargetView* ShaderResourcesManager::RenderTargetView(const size_t id) const {
		RenderTargetViews::const_iterator it = mRenderTargetViews.find(id);
		if (it != mRenderTargetViews.end()) {
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
	
	ID3D11BlendState* ShaderResourcesManager::BlendState(const size_t id) const {
		BlendStates::const_iterator it = mBlendStates.find(id);
		if (it != mBlendStates.end()) {
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