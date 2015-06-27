#include "DrawManager.h"

#include <d3d11_1.h>
#include <vector>
#include <yaml-cpp/yaml.h>

#include <general/Camera.h>
#include <input/Keyboard.h>
#include <managers/ModelManager.h>
#include <managers/ShaderResourcesManager.h>
#include <rendering/GlobalResources.h>
#include <rendering/RenderStateHelper.h>
#include <rendering/models/Mesh.h>
#include <rendering/models/Model.h>
#include <rendering/shaders/VertexType.h>
#include <rendering/shaders/normalDisplacement/ds/NormalDisplacementDsData.h>
#include <rendering/shaders/normalDisplacement/hs/NormalDisplacementHsData.h>
#include <rendering/shaders/normalDisplacement/ps/NormalDisplacementPsData.h>
#include <rendering/shaders/normalDisplacement/vs/NormalDisplacementVsData.h>
#include <rendering/shaders/normalMapping/NormalMappingDrawer.h>
#include <rendering/shaders/normalMapping/vs/NormalMappingVsData.h>
#include <utils/Assert.h>
#include <utils/Memory.h>
#include <utils/Hash.h>
#include <utils/YamlUtils.h>

using namespace DirectX;

namespace BRE {
	DrawManager* DrawManager::gInstance = nullptr;

	DrawManager::DrawManager(ID3D11Device1& device, ID3D11DeviceContext1& context, const unsigned int screenWidth, const unsigned int screenHeight)
		: mPostProcessDrawer(device)
		, mFrameRateDrawer(device, context)
	{
		InitPostProcessResources(screenWidth, screenHeight);
		InitGBuffers(screenWidth, screenHeight);
	}

	void DrawManager::LoadModels(const char* filepath) {
		ASSERT_PTR(filepath);

		const YAML::Node yamlFile = YAML::LoadFile(filepath);
		ASSERT_COND(yamlFile.IsDefined());

		// Get models node
		const YAML::Node nodes = yamlFile["models"];
		ASSERT_COND(nodes.IsDefined());
		ASSERT_COND(nodes.IsSequence());
		
		// Iterate through each node
		for (const YAML::Node& node : nodes) {
			ASSERT_COND(node.IsDefined());
			ASSERT_COND(node.IsMap());

			// Get common rendering attributes
			const std::string modelFilePath = YamlUtils::GetScalar<std::string>(node, "path");
			const std::string renderType = YamlUtils::GetScalar<std::string>(node, "renderType");

			float translation[3];
			YamlUtils::GetSequence<float>(node, "translation", translation, ARRAYSIZE(translation));
			const XMMATRIX translationMatrix = XMMatrixTranslation(translation[0], translation[1], translation[2]);

			float rotation[3];
			YamlUtils::GetSequence<float>(node, "rotation", rotation, ARRAYSIZE(rotation));
			const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation[0], rotation[1], rotation[2]);

			float scaling[3];
			YamlUtils::GetSequence<float>(node, "scaling", scaling, ARRAYSIZE(scaling));
			const XMMATRIX scalingMatrix = XMMatrixScaling(scaling[0], scaling[1], scaling[2]);

			const XMMATRIX worldMatrix = scalingMatrix * rotationMatrix * translationMatrix;

			const std::string material = YamlUtils::GetScalar<std::string>(node, "material");
			const size_t matId = Hash(material.c_str());

			const Model* model;
			const size_t modelId = ModelManager::gInstance->LoadModel(modelFilePath.c_str(), &model);			
			ASSERT_COND(model);
			const std::vector<BRE::Mesh*>& meshes = model->Meshes();
			const size_t numMeshes = meshes.size();
			ASSERT_COND(numMeshes > 0);
			if (renderType == "Normal") {	
				const float textureScaleFactor = YamlUtils::GetScalar<float>(node, "textureScaleFactor");
				ID3D11ShaderResourceView* normalMapSRV = nullptr;
				if (YamlUtils::IsDefined(node, "normalMapTexture")) {
					ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
					const std::string normalMapTexture = YamlUtils::GetScalar<std::string>(node, "normalMapTexture");
					shaderResourcesMgr.AddTextureFromFileSRV(normalMapTexture.c_str(), &normalMapSRV);
				}
				
				for (size_t iMeshIndex = 0; iMeshIndex < numMeshes; ++iMeshIndex) {
					NormalMappingDrawer renderer;
					renderer.VertexShaderData().VertexBuffer() = ShaderResourcesManager::gInstance->Buffer(NormalMappingVertexData::CreateVertexBuffer(modelId, iMeshIndex));
					ASSERT_PTR(renderer.VertexShaderData().VertexBuffer());
					renderer.VertexShaderData().IndexBuffer() = ShaderResourcesManager::gInstance->Buffer(model->CreateIndexBuffer(iMeshIndex));
					renderer.VertexShaderData().SetIndexCount(static_cast<unsigned int>(meshes[iMeshIndex]->Indices().size()));
					renderer.VertexShaderData().TextureScaleFactor() = textureScaleFactor;

					XMStoreFloat4x4(&renderer.WorldMatrix(), worldMatrix);
					
					renderer.PixelShaderData().SetMaterial(matId);
					renderer.PixelShaderData().SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

					if (normalMapSRV) {
						renderer.PixelShaderData().NormalSRV() = normalMapSRV;
					}					

					NormalMappingDrawerVec().push_back(renderer);
				}
			}
			else if (renderType == "Normal_Displacement") {
				const float textureScaleFactor = YamlUtils::GetScalar<float>(node, "textureScaleFactor");
				float edgeTesselationFactors[3];
				YamlUtils::GetSequence<float>(node, "edgeTessellationFactors", edgeTesselationFactors, ARRAYSIZE(edgeTesselationFactors));
				const float insideTessellationFactor = YamlUtils::GetScalar<float>(node, "insideTessellationFactors");
				const float displacementScale = YamlUtils::GetScalar<float>(node, "displacementScale");

				ID3D11ShaderResourceView* displacementSRV;
				{
					ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
					const std::string displacementMapTexture = YamlUtils::GetScalar<std::string>(node, "displacementMapTexture");
					shaderResourcesMgr.AddTextureFromFileSRV(displacementMapTexture.c_str(), &displacementSRV);
					ASSERT_PTR(displacementSRV);
				}
				ID3D11ShaderResourceView* normalMapSRV = nullptr;				
				if (YamlUtils::IsDefined(node, "normalMapTexture")) {
					ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
					const std::string normalMapTexture = YamlUtils::GetScalar<std::string>(node, "normalMapTexture");
					shaderResourcesMgr.AddTextureFromFileSRV(normalMapTexture.c_str(), &normalMapSRV);
				}

				for (size_t iMeshIndex = 0; iMeshIndex < numMeshes; ++iMeshIndex) {
					NormalDisplacementDrawer renderer;
					renderer.VertexShaderData().VertexBuffer() = ShaderResourcesManager::gInstance->Buffer(NormalMappingVertexData::CreateVertexBuffer(modelId, iMeshIndex));
					ASSERT_PTR(renderer.VertexShaderData().VertexBuffer());
					renderer.VertexShaderData().IndexBuffer() = ShaderResourcesManager::gInstance->Buffer(model->CreateIndexBuffer(iMeshIndex));
					renderer.VertexShaderData().SetIndexCount(static_cast<unsigned int>(model->Meshes()[iMeshIndex]->Indices().size()));
					renderer.VertexShaderData().TextureScaleFactor() = textureScaleFactor;

					// Build world matrix
					XMStoreFloat4x4(&renderer.WorldMatrix(), worldMatrix);

					// Initialize hull shader data
					for (size_t iTessFactor = 0; iTessFactor < ARRAYSIZE(edgeTesselationFactors); ++iTessFactor) {
						renderer.HullShaderData().TessellationFactors()[iTessFactor] = edgeTesselationFactors[iTessFactor];
					}
					renderer.HullShaderData().TessellationFactors()[3] = insideTessellationFactor;

					// Initialize domain shader data
					renderer.DomainShaderData().DisplacementScale() = displacementScale;
					renderer.DomainShaderData().DisplacementMapSRV() = displacementSRV;
					renderer.DomainShaderData().SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

					// Initialize pixel shader data
					renderer.PixelShaderData().SetMaterial(matId);
					renderer.PixelShaderData().SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

					if (normalMapSRV) {
						renderer.PixelShaderData().NormalSRV() = normalMapSRV;
					}

					NormalDisplacementDrawerVec().push_back(renderer);
				}
			}
			else if (renderType == "Basic") {
				for (size_t iMeshIndex = 0; iMeshIndex < numMeshes; ++iMeshIndex) {
					BasicDrawer renderer;
					renderer.VertexShaderData().VertexBuffer() = ShaderResourcesManager::gInstance->Buffer(BasicVertexData::CreateVertexBuffer(modelId, iMeshIndex));
					ASSERT_PTR(renderer.VertexShaderData().VertexBuffer());
					renderer.VertexShaderData().IndexBuffer() = ShaderResourcesManager::gInstance->Buffer(model->CreateIndexBuffer(iMeshIndex));
					renderer.VertexShaderData().SetIndexCount(static_cast<unsigned int>(model->Meshes()[iMeshIndex]->Indices().size()));

					XMStoreFloat4x4(&renderer.WorldMatrix(), worldMatrix);

					renderer.PixelShaderData().SetMaterial(matId);
					renderer.PixelShaderData().SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

					BasicDrawerVec().push_back(renderer);
				}
			}
		}
	}

	void DrawManager::DrawAll(ID3D11Device1& device, ID3D11DeviceContext1& context, IDXGISwapChain1& swapChain, ID3D11RenderTargetView& backBufferRTV, ID3D11DepthStencilView& depthStencilView, ID3D11ShaderResourceView& depthStencilSRV) {
		static enum RenderMode {
			BACK_BUFFER,
			NORMAL,
			BASE_COLOR,
			SMOOTHNESS_METALMASK,
			REFLECTANCE,
		} renderMode = BACK_BUFFER;
		if (Keyboard::gInstance->IsKeyDown(DIK_1)) {
			renderMode = BACK_BUFFER;
		}
		else if (Keyboard::gInstance->IsKeyDown(DIK_2)) {
			renderMode = NORMAL;
		}
		else if (Keyboard::gInstance->IsKeyDown(DIK_3)) {
			renderMode = BASE_COLOR;
		}
		else if (Keyboard::gInstance->IsKeyDown(DIK_4)) {
			renderMode = SMOOTHNESS_METALMASK;
		}
		else if (Keyboard::gInstance->IsKeyDown(DIK_5)) {
			renderMode = REFLECTANCE;
		}

		RenderStateHelper::gInstance->SaveAll();

		// Clear render target views
		ID3D11RenderTargetView* backBuffer = &backBufferRTV;
		context.ClearRenderTargetView(&backBufferRTV, reinterpret_cast<const float*>(&Colors::Black));
		context.ClearDepthStencilView(&depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		for (size_t i = 0; i < ARRAYSIZE(mGBuffersSRVs); ++i) {
			context.ClearRenderTargetView(mGBuffersRTVs[i], reinterpret_cast<const float*>(&Colors::Black));
		}

		context.ClearRenderTargetView(mPostprocess1RTV, reinterpret_cast<const float*>(&Colors::Black));
		context.ClearRenderTargetView(mPostprocess2RTV, reinterpret_cast<const float*>(&Colors::Black));

		context.OMSetRenderTargets(1, &backBuffer, &depthStencilView);

		const XMMATRIX view = Camera::gInstance->ViewMatrix();
		const XMMATRIX proj = Camera::gInstance->ProjectionMatrix();
		const float nearClipPlaneDistance = Camera::gInstance->NearPlaneDistance();
		const float farClipPlaneDistance = Camera::gInstance->FarPlaneDistance();
		for (NormalDisplacementDrawer& elem : mNormalDisplacementDrawer) {
			elem.Draw(device, context, mGBuffersRTVs, view, proj);
		}
		for (NormalMappingDrawer& elem : mNormalMappingDrawer) {
			elem.Draw(device, context, mGBuffersRTVs, view, proj);
		}
		for (BasicDrawer& elem : mBasicDrawer) {
			elem.Draw(device, context, mGBuffersRTVs, view, proj);
		}

		if (renderMode == BACK_BUFFER) {
			context.OMSetRenderTargets(1, &mPostprocess1RTV, nullptr);
			mLightsDrawer.Draw(device, context, mGBuffersSRVs, depthStencilSRV, nearClipPlaneDistance, farClipPlaneDistance, view, proj);
			context.OMSetRenderTargets(1, &backBuffer, &depthStencilView);
			mPostProcessDrawer.Draw(device, context, mPostprocess1SRV);	 		
		}
		else {
			ID3D11Texture2D* texture;
			ID3D11Texture2D* backBufferTexture;
			ASSERT_HR(swapChain.GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture)));
			if (renderMode == NORMAL) {
				texture = ShaderResourcesManager::gInstance->Texture2D(Hash("gbuffers_normal"));
			}
			else if (renderMode == BASE_COLOR) {
				texture = ShaderResourcesManager::gInstance->Texture2D(Hash("gbuffers_base_color"));
			}
			else if (renderMode == SMOOTHNESS_METALMASK) {
				texture = ShaderResourcesManager::gInstance->Texture2D(Hash("gbuffers_smoothness_metalmask"));
			}
			else {
				texture = ShaderResourcesManager::gInstance->Texture2D(Hash("gbuffers_reflectance"));
			}

			ASSERT_PTR(texture);
			context.CopyResource(backBufferTexture, texture);
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
		textureDesc[1].Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		textureDesc[1].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc[1].Usage = D3D11_USAGE_DEFAULT;
		textureDesc[1].SampleDesc.Count = 1;
		textureDesc[1].SampleDesc.Quality = 0;

		// Smoothness_MetalMask texture desc
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

		// Reflectance texture desc
		ZeroMemory(&textureDesc[3], sizeof(textureDesc[3]));
		textureDesc[3].Width = screenWidth;
		textureDesc[3].Height = screenHeight;
		textureDesc[3].MipLevels = 1;
		textureDesc[3].ArraySize = 1;
		textureDesc[3].Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		textureDesc[3].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc[3].Usage = D3D11_USAGE_DEFAULT;
		textureDesc[3].SampleDesc.Count = 1;
		textureDesc[3].SampleDesc.Quality = 0;

		//
		// Texture id's
		//
		const char* textureIds[numTextures] = {
			"gbuffers_normal",
			"gbuffers_base_color",
			"gbuffers_smoothness_metalmask",
			"gbuffers_reflectance",
		};

		//
		// Create texture 2D, shader resource view and render target view
		//
		ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
		for (size_t iTex = 0; iTex < numTextures; ++iTex) {
			ID3D11Texture2D* texture;
			shaderResourcesMgr.AddTexture2D(textureIds[iTex], textureDesc[iTex], nullptr, &texture);
			ASSERT_PTR(texture);

			ASSERT_COND(mGBuffersRTVs[iTex] == nullptr);
			shaderResourcesMgr.AddRenderTargetView(textureIds[iTex], *texture, nullptr, &mGBuffersRTVs[iTex]);
			ASSERT_PTR(mGBuffersRTVs[iTex]);

			ASSERT_COND(mGBuffersSRVs[iTex] == nullptr);
			shaderResourcesMgr.AddResourceSRV(textureIds[iTex], *texture, nullptr, &mGBuffersSRVs[iTex]);
			ASSERT_PTR(mGBuffersSRVs[iTex]);
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
		ASSERT_PTR(texture1);

		shaderResourcesMgr.AddRenderTargetView(textureId1, *texture1, nullptr, &mPostprocess1RTV);
		ASSERT_PTR(mPostprocess1RTV);

		shaderResourcesMgr.AddResourceSRV(textureId1, *texture1, nullptr, &mPostprocess1SRV);
		ASSERT_PTR(mPostprocess1SRV);

		ID3D11Texture2D* texture2;
		shaderResourcesMgr.AddTexture2D(textureId2, textureDesc, nullptr, &texture2);
		ASSERT_PTR(texture2);

		shaderResourcesMgr.AddRenderTargetView(textureId2, *texture2, nullptr, &mPostprocess2RTV);
		ASSERT_PTR(mPostprocess2RTV);

		shaderResourcesMgr.AddResourceSRV(textureId2, *texture2, nullptr, &mPostprocess2SRV);
		ASSERT_PTR(mPostprocess2SRV);
	}
}