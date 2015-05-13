#include "DrawManager.h"

#include <boost/lexical_cast.hpp>
#include <d3d11_1.h>
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>

#include <general/Camera.h>
#include <input/Keyboard.h>
#include <managers/ShaderResourcesManager.h>
#include <rendering/GlobalResources.h>
#include <rendering/RenderStateHelper.h>
#include <rendering/models/Mesh.h>
#include <rendering/models/Model.h>
#include <rendering/shaders/normalDisplacement/ds/NormalDisplacementDsData.h>
#include <rendering/shaders/normalDisplacement/hs/NormalDisplacementHsData.h>
#include <rendering/shaders/normalDisplacement/ps/NormalDisplacementPsData.h>
#include <rendering/shaders/normalDisplacement/vs/NormalDisplacementVsData.h>
#include <rendering/shaders/normalMapping/NormalMappingDrawer.h>
#include <rendering/shaders/normalMapping/vs/NormalMappingVsData.h>
#include <utils/Assert.h>
#include <utils/Memory.h>
#include <utils/Utility.h>

using namespace DirectX;

namespace {
	template<typename T>
	T GetScalar(const YAML::Node& node, const char* key) {
		ASSERT_PTR(key);

		YAML::Node attr = node[key];
		ASSERT_COND(attr.IsDefined());
		ASSERT_COND(attr.IsScalar());
		return boost::lexical_cast<T>(attr.as<std::string>());
	}

	template<>
	std::string GetScalar(const YAML::Node& node, const char* key) {
		ASSERT_PTR(key);

		YAML::Node attr = node[key];
		ASSERT_COND(attr.IsDefined());
		ASSERT_COND(attr.IsScalar());
		return attr.as<std::string>();
	}

	template<typename T>
#ifdef _DEBUG
	void GetSequence(const YAML::Node& node, const char* key, T* const sequence, const size_t numElems) {
#else
	void GetSequence(const YAML::Node& node, const char* key, T* const sequence, const size_t) {
#endif
		ASSERT_PTR(key);
		ASSERT_PTR(sequence);

		YAML::Node attr = node[key];
		ASSERT_COND(attr.IsDefined());
		ASSERT_COND(attr.IsSequence());
		size_t currentNumElems = 0;
		for (const YAML::Node& seqNode : attr) {
			ASSERT_COND(seqNode.IsScalar());
			ASSERT_COND(currentNumElems < numElems);
			sequence[currentNumElems] = seqNode.as<T>();
			++currentNumElems;
		}
		ASSERT_COND(currentNumElems == numElems);
	}

	template<>
#ifdef _DEBUG
	void GetSequence(const YAML::Node& node, const char* key, float* const sequence, const size_t numElems) {
#else
	void GetSequence(const YAML::Node& node, const char* key, float* const sequence, const size_t) {
#endif
		ASSERT_PTR(key);
		ASSERT_PTR(sequence);

		YAML::Node attr = node[key];
		ASSERT_COND(attr.IsDefined());
		ASSERT_COND(attr.IsSequence());
		size_t currentNumElems = 0;
		for (const YAML::Node& seqNode : attr) {
			ASSERT_COND(seqNode.IsScalar());
			ASSERT_COND(currentNumElems < numElems);
			sequence[currentNumElems] = boost::lexical_cast<float>(seqNode.as<std::string>());
			++currentNumElems;
		}
		ASSERT_COND(currentNumElems == numElems);
	}

	void GetVerticesAndIndices(BRE::Model& model, std::vector<BRE::NormalMappingVsData::VertexData>& vertices, std::vector<unsigned int>& indices) {
		vertices.clear();
		indices.clear();

		// Read data from model
		ASSERT_PTR(model.Meshes()[0]);
		BRE::Mesh& mesh = *model.Meshes()[0];
		const std::vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
		std::vector<XMFLOAT3>* textureCoordinates = mesh.TextureCoordinates().at(0);
		ASSERT_COND(textureCoordinates->size() == sourceVertices.size());
		const std::vector<XMFLOAT3>& normals = mesh.Normals();
		ASSERT_COND(normals.size() == sourceVertices.size());
		const std::vector<XMFLOAT3>& tangents = mesh.Tangents();
		ASSERT_COND(tangents.size() == sourceVertices.size());

		const size_t numVerts = sourceVertices.size();
		vertices.reserve(numVerts);
		for (size_t i = 0; i < numVerts; i++) {
			const XMFLOAT3& posL = sourceVertices[i];
			const XMFLOAT3& uv = (*textureCoordinates)[i];
			const XMFLOAT3& normalL = normals[i];
			const XMFLOAT3& tangentL = tangents[i];
			vertices.push_back(BRE::NormalMappingVsData::VertexData(XMFLOAT4(posL.x, posL.y, posL.z, 1.0f), XMFLOAT2(uv.x, uv.y), normalL, tangentL));
		}

		indices.insert(indices.end(), mesh.Indices().begin(), mesh.Indices().end());
	}

	void GetVerticesAndIndices(BRE::Model& model, std::vector<BRE::NormalDisplacementVsData::VertexData>& vertices, std::vector<unsigned int>& indices) {
		vertices.clear();
		indices.clear();

		// Read data from model
		ASSERT_PTR(model.Meshes()[0]);
		BRE::Mesh& mesh = *model.Meshes()[0];
		const std::vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
		std::vector<XMFLOAT3>* textureCoordinates = mesh.TextureCoordinates().at(0);
		ASSERT_COND(textureCoordinates->size() == sourceVertices.size());
		const std::vector<XMFLOAT3>& normals = mesh.Normals();
		ASSERT_COND(normals.size() == sourceVertices.size());
		const std::vector<XMFLOAT3>& tangents = mesh.Tangents();
		ASSERT_COND(tangents.size() == sourceVertices.size());

		const size_t numVerts = sourceVertices.size();
		vertices.reserve(numVerts);
		for (size_t i = 0; i < numVerts; i++) {
			const XMFLOAT3& posL = sourceVertices[i];
			const XMFLOAT3& uv = (*textureCoordinates)[i];
			const XMFLOAT3& normalL = normals[i];
			const XMFLOAT3& tangentL = tangents[i];
			vertices.push_back(BRE::NormalDisplacementVsData::VertexData(XMFLOAT4(posL.x, posL.y, posL.z, 1.0f), XMFLOAT2(uv.x, uv.y), normalL, tangentL));
		}

		indices.insert(indices.end(), mesh.Indices().begin(), mesh.Indices().end());
	}

	ID3D11Buffer* CreateInitializedBuffer(const size_t id, const void* data, const unsigned int dataSize, const D3D11_USAGE usage, const unsigned int bindFlags) {
		ASSERT_PTR(data);
		ASSERT_COND(dataSize > 0);

		// Create vertex buffer
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.ByteWidth = dataSize;
		bufferDesc.Usage = usage;
		bufferDesc.BindFlags = bindFlags;

		D3D11_SUBRESOURCE_DATA subResourceData;
		ZeroMemory(&subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
		subResourceData.pSysMem = data;

		return BRE::ShaderResourcesManager::gInstance->AddBuffer(id, bufferDesc, &subResourceData);
	}

	ID3D11Buffer* CreateNonInitializedBuffer(const size_t id, const unsigned int dataSize, const D3D11_USAGE usage, const unsigned int bindFlags) {
		ASSERT_COND(dataSize > 0);

		// Create vertex buffer
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.ByteWidth = dataSize;
		bufferDesc.Usage = usage;
		bufferDesc.BindFlags = bindFlags;

		return BRE::ShaderResourcesManager::gInstance->AddBuffer(id, bufferDesc, nullptr);
	}
	}

namespace BRE {
	DrawManager* DrawManager::gInstance = nullptr;

	DrawManager::DrawManager(ID3D11Device1& device, ID3D11DeviceContext1& context, const unsigned int screenWidth, const unsigned int screenHeight)
		: mFrameRateDrawer(device, context)
	{
		InitResources(screenWidth, screenHeight);
		InitPostProcessResources(screenWidth, screenHeight);
	}

	void DrawManager::LoadModels(const char* filepath) {
		ASSERT_PTR(filepath);

		const YAML::Node yamlFile = YAML::LoadFile(filepath);
		ASSERT_COND(yamlFile.IsDefined());

		// Get models node
		const YAML::Node nodes = yamlFile["models"];
		ASSERT_COND(nodes.IsDefined());
		ASSERT_COND(nodes.IsSequence());

		// Store models loaded in file reading process to avoid
		// loading them more than once.
		std::unordered_map<size_t, Model*> modelByFilepath;

		// Iterate through each node
		for (const YAML::Node& node : nodes) {
			ASSERT_COND(node.IsDefined());
			ASSERT_COND(node.IsMap());

			// Get common rendering attributes
			const std::string modelFilePath = GetScalar<std::string>(node, "path");
			const std::string renderType = GetScalar<std::string>(node, "renderType");
			const std::string diffuseMapTexture = GetScalar<std::string>(node, "diffuseMapTexture");

			float translation[3];
			GetSequence<float>(node, "translation", translation, ARRAYSIZE(translation));
			const XMMATRIX translationMatrix = XMMatrixTranslation(translation[0], translation[1], translation[2]);

			float rotation[3];
			GetSequence<float>(node, "rotation", rotation, ARRAYSIZE(rotation));
			const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation[0], rotation[1], rotation[2]);

			float scaling[3];
			GetSequence<float>(node, "scaling", scaling, ARRAYSIZE(scaling));
			const XMMATRIX scalingMatrix = XMMatrixScaling(scaling[0], scaling[1], scaling[2]);

			std::vector<NormalMappingVsData::VertexData> normalMappingVertices;
			std::vector<NormalDisplacementVsData::VertexData> normalDisplacementVertices;
			std::vector<unsigned int> indices;
			Model* &model = modelByFilepath[Utility::Hash(modelFilePath.c_str())];
			if (!model) {
				model = new BRE::Model(modelFilePath.c_str());
			}

			if (renderType == "Normal") {
				NormalMappingDrawer renderer;

				// Get vertices and indices
				GetVerticesAndIndices(*model, normalMappingVertices, indices);
				ASSERT_COND(!normalMappingVertices.empty());
				ASSERT_COND(!indices.empty());

				// Create vertex buffer
				std::string bufferName(modelFilePath);
				bufferName += "_vertexBuffer";
				size_t bufferId = Utility::Hash(bufferName.c_str());
				ID3D11Buffer* &vertexBuffer = renderer.VertexShaderData().VertexBuffer();
				vertexBuffer = ShaderResourcesManager::gInstance->Buffer(bufferId);
				if (!vertexBuffer) {
					const unsigned int bufferSize = static_cast<unsigned int> (normalMappingVertices.size() * sizeof(NormalMappingVsData::VertexData));
					vertexBuffer = CreateInitializedBuffer(bufferId, &normalMappingVertices[0], bufferSize, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER);
					ASSERT_PTR(vertexBuffer);
				}

				// Create index buffer
				bufferName = modelFilePath;
				bufferName += "_indexBuffer";
				bufferId = Utility::Hash(bufferName.c_str());
				ID3D11Buffer* &indexBuffer = renderer.VertexShaderData().IndexBuffer();
				indexBuffer = ShaderResourcesManager::gInstance->Buffer(bufferId);
				if (!indexBuffer) {
					const unsigned int bufferSize = static_cast<unsigned int> (indices.size() * sizeof(unsigned int));
					indexBuffer = CreateInitializedBuffer(bufferId, &indices[0], bufferSize, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER);
					ASSERT_PTR(indexBuffer);
				}

				renderer.VertexShaderData().SetIndexCount(static_cast<unsigned int>(indices.size()));

				// Build world matrix
				XMStoreFloat4x4(&renderer.WorldMatrix(), scalingMatrix * rotationMatrix * translationMatrix);

				// Initialize pixel shader data
				ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
				renderer.PixelShaderData().DiffuseTextureSRV() = shaderResourcesMgr.AddTextureFromFileSRV(diffuseMapTexture.c_str());
				ASSERT_PTR(renderer.PixelShaderData().DiffuseTextureSRV());
				const std::string normalMapTexture = GetScalar<std::string>(node, "normalMapTexture");
				renderer.PixelShaderData().NormalMapTextureSRV() = shaderResourcesMgr.AddTextureFromFileSRV(normalMapTexture.c_str());
				ASSERT_PTR(renderer.PixelShaderData().NormalMapTextureSRV());
				const std::string specularMapTexture = GetScalar<std::string>(node, "specularMapTexture");
				renderer.PixelShaderData().SpecularMapTextureSRV() = shaderResourcesMgr.AddTextureFromFileSRV(specularMapTexture.c_str());
				ASSERT_PTR(renderer.PixelShaderData().SpecularMapTextureSRV());
				renderer.PixelShaderData().SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

				NormalMappingDrawerVec().push_back(renderer);
			}
			else if (renderType == "Normal_Displacement") {
				NormalDisplacementDrawer renderer;

				// Get vertices and indices
				GetVerticesAndIndices(*model, normalDisplacementVertices, indices);
				ASSERT_COND(!normalDisplacementVertices.empty());
				ASSERT_COND(!indices.empty());

				// Create vertex buffer
				std::string bufferName(modelFilePath);
				bufferName += "_vertexBuffer";
				size_t bufferId = Utility::Hash(bufferName.c_str());
				ID3D11Buffer* &vertexBuffer = renderer.VertexShaderData().VertexBuffer();
				vertexBuffer = ShaderResourcesManager::gInstance->Buffer(bufferId);
				if (!vertexBuffer) {
					const unsigned int bufferSize = static_cast<unsigned int> (normalDisplacementVertices.size() * sizeof(NormalDisplacementVsData::VertexData));
					vertexBuffer = CreateInitializedBuffer(bufferId, &normalDisplacementVertices[0], bufferSize, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER);
					ASSERT_PTR(vertexBuffer);
				}

				// Create index buffer
				bufferName = modelFilePath;
				bufferName += "_indexBuffer";
				bufferId = Utility::Hash(bufferName.c_str());
				ID3D11Buffer* &indexBuffer = renderer.VertexShaderData().IndexBuffer();
				indexBuffer = ShaderResourcesManager::gInstance->Buffer(bufferId);
				if (!indexBuffer) {
					const unsigned int bufferSize = static_cast<unsigned int> (indices.size() * sizeof(unsigned int));
					indexBuffer = CreateInitializedBuffer(bufferId, &indices[0], bufferSize, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER);
					ASSERT_PTR(indexBuffer);
				}

				renderer.VertexShaderData().SetIndexCount(static_cast<unsigned int> (indices.size()));

				// Build world matrix
				XMStoreFloat4x4(&renderer.WorldMatrix(), scalingMatrix * rotationMatrix * translationMatrix);

				// Initialize hull shader data
				GetSequence<float>(node, "edgeTesselationFactors", renderer.HullShaderData().TessellationFactors(), 3);
				renderer.HullShaderData().TessellationFactors()[3] = GetScalar<float>(node, "insideTessellationFactors");

				// Initialize domain shader data
				renderer.DomainShaderData().DisplacementScale() = GetScalar<float>(node, "displacementScale");
				ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
				const std::string displacementMapTexture = GetScalar<std::string>(node, "displacementMapTexture");
				renderer.DomainShaderData().DisplacementMapSRV() = shaderResourcesMgr.AddTextureFromFileSRV(displacementMapTexture.c_str());
				ASSERT_PTR(renderer.DomainShaderData().DisplacementMapSRV());
				renderer.DomainShaderData().SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

				// Initialize pixel shader data
				renderer.PixelShaderData().DiffuseTextureSRV() = shaderResourcesMgr.AddTextureFromFileSRV(diffuseMapTexture.c_str());
				ASSERT_PTR(renderer.PixelShaderData().DiffuseTextureSRV());
				const std::string normalMapTexture = GetScalar<std::string>(node, "normalMapTexture");
				renderer.PixelShaderData().NormalMapTextureSRV() = shaderResourcesMgr.AddTextureFromFileSRV(normalMapTexture.c_str());
				ASSERT_PTR(renderer.PixelShaderData().NormalMapTextureSRV());
				const std::string specularMapTexture = GetScalar<std::string>(node, "specularMapTexture");
				renderer.PixelShaderData().SpecularMapTextureSRV() = shaderResourcesMgr.AddTextureFromFileSRV(specularMapTexture.c_str());
				ASSERT_PTR(renderer.PixelShaderData().SpecularMapTextureSRV());
				renderer.PixelShaderData().SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

				NormalDisplacementDrawerVec().push_back(renderer);
			}
		}

		// Free models
		for (auto& elem : modelByFilepath) {
			DELETE_OBJECT(elem.second);
		}
	}

	void DrawManager::DrawAll(ID3D11Device1& device, ID3D11DeviceContext1& context, IDXGISwapChain1& swapChain, ID3D11RenderTargetView& backBufferRTV, ID3D11DepthStencilView& depthStencilView) {
		static enum RenderMode {
			BACK_BUFFER,
			DIFFUSE_ALBEDO,
			SPECULAR_ALBEDO,
			NORMALS,
			DEPTH
		} renderMode = BACK_BUFFER;
		if (Keyboard::gInstance->IsKeyDown(DIK_1)) {
			renderMode = BACK_BUFFER;
		}
		else if (Keyboard::gInstance->IsKeyDown(DIK_2)) {
			renderMode = DIFFUSE_ALBEDO;
		}
		else if (Keyboard::gInstance->IsKeyDown(DIK_3)) {
			renderMode = SPECULAR_ALBEDO;
		}
		else if (Keyboard::gInstance->IsKeyDown(DIK_4)) {
			renderMode = NORMALS;
		}
		else if (Keyboard::gInstance->IsKeyDown(DIK_5)) {
			renderMode = DEPTH;
		}

		RenderStateHelper::gInstance->SaveAll();

		// Clear render target views
		ID3D11RenderTargetView* backBuffer = &backBufferRTV;
		context.ClearRenderTargetView(&backBufferRTV, reinterpret_cast<const float*>(&Colors::Black));
		context.ClearDepthStencilView(&depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		const size_t numGeometryBuffersRTVs = ARRAYSIZE(mGeometryBuffersRTVs);
		for (size_t i = 0; i < numGeometryBuffersRTVs; ++i) {
			context.ClearRenderTargetView(mGeometryBuffersRTVs[i], reinterpret_cast<const float*>(&Colors::Black));
		}

		context.OMSetRenderTargets(1, &backBuffer, &depthStencilView);

		const XMMATRIX view = Camera::gInstance->ViewMatrix();
		const XMMATRIX proj = Camera::gInstance->ProjectionMatrix();
		for (NormalDisplacementDrawer& elem : mNormalDisplacementDrawer) {
			elem.Draw(device, context, mGeometryBuffersRTVs, view, proj);
		}
		for (NormalMappingDrawer& elem : mNormalMappingDrawer) {
			elem.Draw(device, context, mGeometryBuffersRTVs, view, proj);
		}

		if (renderMode == BACK_BUFFER) {
			mLightsDrawer.Draw(device, context, mGeometryBuffersSRVs);
		}
		else {
			ID3D11Texture2D* texture;
			ID3D11Texture2D* backBufferTexture;
			ASSERT_HR(swapChain.GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture)));
			if (renderMode == DIFFUSE_ALBEDO) {
				texture = ShaderResourcesManager::gInstance->Texture2D("deferred_rendering_texture2d_diffuse_albedo");
			}
			else if (renderMode == SPECULAR_ALBEDO) {
				texture = ShaderResourcesManager::gInstance->Texture2D("deferred_rendering_texture2d_specular_albedo");
			}
			else if (renderMode == NORMALS) {
				texture = ShaderResourcesManager::gInstance->Texture2D("deferred_rendering_texture2d_normals");
			}
			else {
				texture = ShaderResourcesManager::gInstance->Texture2D("deferred_rendering_texture2d_depth");
			}

			ASSERT_PTR(texture);
			context.CopyResource(backBufferTexture, texture);
		}

		mFrameRateDrawer.Draw();

		ASSERT_HR(swapChain.Present(0, 0));

		RenderStateHelper::gInstance->RestoreAll();
	}

	void DrawManager::InitResources(const unsigned int screenWidth, const unsigned int screenHeight) {
		const size_t numTextures = ARRAYSIZE(mGeometryBuffersRTVs);

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
		textureDesc[0].Format = DXGI_FORMAT_R8G8B8A8_SNORM;
		textureDesc[0].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc[0].Usage = D3D11_USAGE_DEFAULT;
		textureDesc[0].SampleDesc.Count = 1;
		textureDesc[0].SampleDesc.Quality = 0;

		// Diffuse albedo texture desc
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

		// Specular albedo texture desc
		ZeroMemory(&textureDesc[2], sizeof(textureDesc[2]));
		textureDesc[2].Width = screenWidth;
		textureDesc[2].Height = screenHeight;
		textureDesc[2].MipLevels = 1;
		textureDesc[2].ArraySize = 1;
		textureDesc[2].Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		textureDesc[2].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc[2].Usage = D3D11_USAGE_DEFAULT;
		textureDesc[2].SampleDesc.Count = 1;
		textureDesc[2].SampleDesc.Quality = 0;

		// Depth texture description
		ZeroMemory(&textureDesc[3], sizeof(textureDesc[3]));
		textureDesc[3].Width = screenWidth;
		textureDesc[3].Height = screenHeight;
		textureDesc[3].MipLevels = 1;
		textureDesc[3].ArraySize = 1;
		textureDesc[3].Format = DXGI_FORMAT_R16_UNORM;
		textureDesc[3].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc[3].Usage = D3D11_USAGE_DEFAULT;
		textureDesc[3].SampleDesc.Count = 1;
		textureDesc[3].SampleDesc.Quality = 0;

		//
		// Texture id's
		//
		const char* textureIds[numTextures] = {
			"deferred_rendering_texture2d_normals",
			"deferred_rendering_texture2d_diffuse_albedo",
			"deferred_rendering_texture2d_specular_albedo",
			"deferred_rendering_texture2d_depth"
		};

		//
		// Create texture 2D, shader resource view and render target view
		//
		ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
		for (size_t iTex = 0; iTex < numTextures; ++iTex) {
			ID3D11Texture2D* texture = shaderResourcesMgr.AddTexture2D(textureIds[iTex], textureDesc[iTex], nullptr);
			ASSERT_PTR(texture);

			ASSERT_COND(mGeometryBuffersRTVs[iTex] == nullptr);
			mGeometryBuffersRTVs[iTex] = shaderResourcesMgr.AddRenderTargetView(textureIds[iTex], *texture, nullptr);
			ASSERT_PTR(mGeometryBuffersRTVs[iTex]);

			ASSERT_COND(mGeometryBuffersSRVs[iTex] == nullptr);
			mGeometryBuffersSRVs[iTex] = shaderResourcesMgr.AddResourceSRV(textureIds[iTex], *texture, nullptr);
			ASSERT_PTR(mGeometryBuffersSRVs[iTex]);
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
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;

		// Texture id
		const char* textureId1 = "postprocess_texture2d_1";
		const char* textureId2 = "postprocess_texture2d_2";

		// Create texture 2D, shader resource view and render target view
		ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
		ID3D11Texture2D* texture1 = shaderResourcesMgr.AddTexture2D(textureId1, textureDesc, nullptr);
		ASSERT_PTR(texture1);

		mPostprocess1RTV = shaderResourcesMgr.AddRenderTargetView(textureId1, *texture1, nullptr);
		ASSERT_PTR(mPostprocess1RTV);

		mPostprocess1SRV = shaderResourcesMgr.AddResourceSRV(textureId1, *texture1, nullptr);
		ASSERT_PTR(mPostprocess1SRV);

		ID3D11Texture2D* texture2 = shaderResourcesMgr.AddTexture2D(textureId2, textureDesc, nullptr);
		ASSERT_PTR(texture2);

		mPostprocess2RTV = shaderResourcesMgr.AddRenderTargetView(textureId2, *texture2, nullptr);
		ASSERT_PTR(mPostprocess2RTV);

		mPostprocess2SRV = shaderResourcesMgr.AddResourceSRV(textureId2, *texture2, nullptr);
		ASSERT_PTR(mPostprocess2SRV);
	}
}