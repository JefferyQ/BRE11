#include "NormalMappingDrawer.h"

#include <yaml-cpp/yaml.h>

#include <managers/ModelManager.h>
#include <rendering/GlobalResources.h>
#include <rendering/models/Mesh.h>
#include <rendering/models/Model.h>
#include <rendering/shaders/VertexType.h>

#include <utils/Assert.h>
#include <utils/Hash.h>
#include <utils/YamlUtils.h>

using namespace DirectX;

namespace BRE {
	void NormalMappingDrawer::Create(const YAML::Node& node, std::vector<NormalMappingDrawer>& drawers) {
		BRE_ASSERT(YamlUtils::GetScalar<std::string>(node, "renderType") == std::string("Normal"));

		const std::string modelFilePath = YamlUtils::GetScalar<std::string>(node, "path");
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
		const float textureScaleFactor = YamlUtils::GetScalar<float>(node, "textureScaleFactor");
		ID3D11ShaderResourceView* normalMapSRV = nullptr;
		if (YamlUtils::IsDefined(node, "normalMapTexture")) {
			ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
			const std::string normalMapTexture = YamlUtils::GetScalar<std::string>(node, "normalMapTexture");
			shaderResourcesMgr.AddTextureFromFileSRV(normalMapTexture.c_str(), &normalMapSRV);
		}

		const std::string material = YamlUtils::GetScalar<std::string>(node, "material");
		const size_t matId = Utils::Hash(material.c_str());

		const Model* model;
		const size_t modelId = ModelManager::gInstance->LoadModel(modelFilePath.c_str(), &model);
		BRE_ASSERT(model);
		const std::vector<BRE::Mesh*>& meshes = model->Meshes();
		const size_t numMeshes = meshes.size();
		BRE_ASSERT(numMeshes > 0);
		for (size_t iMeshIndex = 0; iMeshIndex < numMeshes; ++iMeshIndex) {
			NormalMappingDrawer drawer;
			drawer.mVertexShaderData.VertexBuffer() = ShaderResourcesManager::gInstance->Buffer(NormalMappingVertexData::CreateVertexBuffer(modelId, iMeshIndex));
			BRE_ASSERT(drawer.mVertexShaderData.VertexBuffer());
			drawer.mVertexShaderData.IndexBuffer() = ShaderResourcesManager::gInstance->Buffer(model->CreateIndexBuffer(iMeshIndex));
			drawer.mVertexShaderData.SetIndexCount(static_cast<unsigned int>(model->Meshes()[iMeshIndex]->Indices().size()));
			drawer.mVertexShaderData.TextureScaleFactor() = textureScaleFactor;
			XMStoreFloat4x4(&drawer.mWorld, worldMatrix);
			drawer.mPixelShaderData.SetMaterial(matId);
			drawer.mPixelShaderData.SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();
			if (normalMapSRV) {
				drawer.mPixelShaderData.NormalSRV() = normalMapSRV;
			}
			drawers.push_back(drawer);
		}
	}

	void NormalMappingDrawer::Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs, const XMMATRIX& view, const XMMATRIX& proj) {
		const XMMATRIX world = XMLoadFloat4x4(&mWorld);
		XMStoreFloat4x4(&mVertexShaderData.WorldView(), XMMatrixTranspose(world * view));
		XMStoreFloat4x4(&mVertexShaderData.WorldViewProjection(), XMMatrixTranspose(world * view * proj));

		mVertexShaderData.PreDraw(device, context);
		mPixelShaderData.PreDraw(device, context, geometryBuffersRTVs);
		mVertexShaderData.DrawIndexed(context);
		mVertexShaderData.PostDraw(context);
		mPixelShaderData.PostDraw(context);
	}
}