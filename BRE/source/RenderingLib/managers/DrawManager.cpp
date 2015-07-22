#include "DrawManager.h"

#include <d3d11_1.h>
#include <vector>
#include <yaml-cpp/yaml.h>

#include <general/Camera.h>
#include <managers/ShaderResourcesManager.h>
#include <rendering/RenderStateHelper.h>
#include <utils/Assert.h>
#include <utils/YamlUtils.h>

using namespace DirectX;

namespace BRE {
	DrawManager* DrawManager::gInstance = nullptr;

	DrawManager::DrawManager(ID3D11Device1& device, ID3D11DeviceContext1& context, const unsigned int screenWidth, const unsigned int screenHeight)
		: mPostProcessDrawer(device)
		, mFrameRateDrawer(device, context)
	{
		InitGBuffers(screenWidth, screenHeight);
		InitPostProcessResources(screenWidth, screenHeight);		
	}

	void DrawManager::LoadModels(const char* filepath) {
		BRE_ASSERT(filepath);

		const YAML::Node yamlFile = YAML::LoadFile(filepath);
		BRE_ASSERT(yamlFile.IsDefined());

		// Get models node
		const YAML::Node nodes = yamlFile["models"];
		BRE_ASSERT(nodes.IsDefined());
		BRE_ASSERT(nodes.IsSequence());
		
		// Iterate through each node
		for (const YAML::Node& node : nodes) {
			BRE_ASSERT(node.IsDefined());
			BRE_ASSERT(node.IsMap());
			const std::string renderType = YamlUtils::GetScalar<std::string>(node, "renderType");
			if (renderType == "Normal") {	
				NormalMappingDrawer::Create(node, mNormalMappingDrawers);
			}
			else if (renderType == "Normal_Displacement") {
				NormalDisplacementDrawer::Create(node, mNormalDisplacementDrawers);
			}
			else if (renderType == "Basic") {
				BasicDrawer::Create(node, mBasicDrawers);
			}
		}
	}

	void DrawManager::DrawAll(ID3D11Device1& device, ID3D11DeviceContext1& context, IDXGISwapChain1& swapChain, ID3D11RenderTargetView& backBufferRTV, ID3D11DepthStencilView& depthStencilView, ID3D11ShaderResourceView& depthStencilSRV) {
		RenderStateHelper::gInstance->SaveAll();

		// Clear render target views
		ID3D11RenderTargetView* backBuffer = &backBufferRTV;
		{
			context.ClearRenderTargetView(&backBufferRTV, reinterpret_cast<const float*>(&Colors::Black));
			context.ClearDepthStencilView(&depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			for (size_t i = 0; i < ARRAYSIZE(mGBuffersSRVs); ++i) {
				context.ClearRenderTargetView(mGBuffersRTVs[i], reinterpret_cast<const float*>(&Colors::Black));
			}
			context.ClearRenderTargetView(mPostprocess1RTV, reinterpret_cast<const float*>(&Colors::Black));
			context.ClearRenderTargetView(mPostprocess2RTV, reinterpret_cast<const float*>(&Colors::Black));
		}
		
		// Geometry pass
		const XMMATRIX view = Camera::gInstance->ViewMatrix();
		const XMMATRIX proj = Camera::gInstance->ProjectionMatrix();
		{			
			context.OMSetRenderTargets(1, &backBuffer, &depthStencilView);
			for (NormalMappingDrawer& elem : mNormalMappingDrawers) {
				elem.Draw(device, context, mGBuffersRTVs, view, proj);
			}
			for (NormalDisplacementDrawer& elem : mNormalDisplacementDrawers) {
				elem.Draw(device, context, mGBuffersRTVs, view, proj);
			}
			for (BasicDrawer& elem : mBasicDrawers) {
				elem.Draw(device, context, mGBuffersRTVs, view, proj);
			}
		}

		// Lighting pass
		{
			context.OMSetRenderTargets(1, &mPostprocess1RTV, nullptr);
			const float nearClipPlaneDistance = Camera::gInstance->NearPlaneDistance();
			const float farClipPlaneDistance = Camera::gInstance->FarPlaneDistance();
			mLightsDrawer.Draw(device, context, mGBuffersSRVs, depthStencilSRV, nearClipPlaneDistance, farClipPlaneDistance, view, proj);
		}

		// Post-process pass
		{
			context.OMSetRenderTargets(1, &backBuffer, &depthStencilView);
			mPostProcessDrawer.Draw(device, context, mPostprocess1SRV);
		}

		mFrameRateDrawer.Draw();

		ASSERT_HR(swapChain.Present(0, 0));

		RenderStateHelper::gInstance->RestoreAll();
	}

	void DrawManager::InitGBuffers(const unsigned int screenWidth, const unsigned int screenHeight) {
		const size_t numTextures = ARRAYSIZE(mGBuffersSRVs);

		//
		// Texture descriptions
		//
		D3D11_TEXTURE2D_DESC textureDesc[numTextures];

		// Normal texture desc
		ZeroMemory(&textureDesc[0], sizeof(textureDesc[0]));
		textureDesc[0].Width = screenWidth;
		textureDesc[0].Height = screenHeight;
		textureDesc[0].MipLevels = 1; 
		textureDesc[0].ArraySize = 1;
		textureDesc[0].Format = DXGI_FORMAT_R16G16_FLOAT;
		textureDesc[0].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc[0].Usage = D3D11_USAGE_DEFAULT;
		textureDesc[0].SampleDesc.Count = 1;
		textureDesc[0].SampleDesc.Quality = 0;

		// Base color texture desc
		ZeroMemory(&textureDesc[1], sizeof(textureDesc[1]));
		textureDesc[1].Width = screenWidth;
		textureDesc[1].Height = screenHeight;
		textureDesc[1].MipLevels = 1;
		textureDesc[1].ArraySize = 1;
		textureDesc[1].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc[1].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc[1].Usage = D3D11_USAGE_DEFAULT;
		textureDesc[1].SampleDesc.Count = 1;
		textureDesc[1].SampleDesc.Quality = 0;

		// Smoothness_MetalMask_Curvature texture desc
		ZeroMemory(&textureDesc[2], sizeof(textureDesc[2]));
		textureDesc[2].Width = screenWidth;
		textureDesc[2].Height = screenHeight;
		textureDesc[2].MipLevels = 1;
		textureDesc[2].ArraySize = 1;
		textureDesc[2].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc[2].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc[2].Usage = D3D11_USAGE_DEFAULT;
		textureDesc[2].SampleDesc.Count = 1;
		textureDesc[2].SampleDesc.Quality = 0;

		//
		// Texture id's
		//
		const char* textureIds[numTextures] = {
			"gbuffers_normal",
			"gbuffers_base_color",
			"gbuffers_smoothness_metalmask_curvature",
		};

		//
		// Create texture 2D, shader resource view and render target view
		//
		ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
		for (size_t iTex = 0; iTex < numTextures; ++iTex) {
			ID3D11Texture2D* texture;
			shaderResourcesMgr.AddTexture2D(textureIds[iTex], textureDesc[iTex], nullptr, &texture);
			BRE_ASSERT(texture);

			BRE_ASSERT(mGBuffersRTVs[iTex] == nullptr);
			shaderResourcesMgr.AddRenderTargetView(textureIds[iTex], *texture, nullptr, &mGBuffersRTVs[iTex]);
			BRE_ASSERT(mGBuffersRTVs[iTex]);

			BRE_ASSERT(mGBuffersSRVs[iTex] == nullptr);
			shaderResourcesMgr.AddResourceSRV(textureIds[iTex], *texture, nullptr, &mGBuffersSRVs[iTex]);
			BRE_ASSERT(mGBuffersSRVs[iTex]);
		}
	}

	void DrawManager::InitPostProcessResources(const unsigned int screenWidth, const unsigned int screenHeight) {
		// Texture description
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = screenWidth;
		textureDesc.Height = screenHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;

		// Texture id
		const char* textureId1 = "postprocess_texture2d_1";
		const char* textureId2 = "postprocess_texture2d_2";

		// Create texture 2D, shader resource view and render target view
		ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
		ID3D11Texture2D* texture1;
		shaderResourcesMgr.AddTexture2D(textureId1, textureDesc, nullptr, &texture1);
		BRE_ASSERT(texture1);

		shaderResourcesMgr.AddRenderTargetView(textureId1, *texture1, nullptr, &mPostprocess1RTV);
		BRE_ASSERT(mPostprocess1RTV);

		shaderResourcesMgr.AddResourceSRV(textureId1, *texture1, nullptr, &mPostprocess1SRV);
		BRE_ASSERT(mPostprocess1SRV);

		ID3D11Texture2D* texture2;
		shaderResourcesMgr.AddTexture2D(textureId2, textureDesc, nullptr, &texture2);
		BRE_ASSERT(texture2);

		shaderResourcesMgr.AddRenderTargetView(textureId2, *texture2, nullptr, &mPostprocess2RTV);
		BRE_ASSERT(mPostprocess2RTV);

		shaderResourcesMgr.AddResourceSRV(textureId2, *texture2, nullptr, &mPostprocess2SRV);
		BRE_ASSERT(mPostprocess2SRV);
	}
}