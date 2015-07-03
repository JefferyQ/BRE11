#include "NormalDisplacementDrawer.h"

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
	void NormalDisplacementDrawer::Create(const YAML::Node& node, std::vector<NormalDisplacementDrawer>& drawers) {
		BRE_ASSERT(YamlUtils::GetScalar<std::string>(node, "renderType") == std::string("Normal_Displacement"));

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
		float edgeTesselationFactors[3];
		YamlUtils::GetSequence<float>(node, "edgeTessellationFactors", edgeTesselationFactors, ARRAYSIZE(edgeTesselationFactors));
		const float insideTessellationFactor = YamlUtils::GetScalar<float>(node, "insideTessellationFactors");
		const float displacementScale = YamlUtils::GetScalar<float>(node, "displacementScale");
		ID3D11ShaderResourceView* displacementSRV;
		{
			ShaderResourcesManager& shaderResourcesMgr = *ShaderResourcesManager::gInstance;
			const std::string displacementMapTexture = YamlUtils::GetScalar<std::string>(node, "displacementMapTexture");
			shaderResourcesMgr.AddTextureFromFileSRV(displacementMapTexture.c_str(), &displacementSRV);
			BRE_ASSERT(displacementSRV);
		}
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
			NormalDisplacementDrawer drawer;
			drawer.mVertexShaderData.VertexBuffer() = ShaderResourcesManager::gInstance->Buffer(NormalMappingVertexData::CreateVertexBuffer(modelId, iMeshIndex));
			BRE_ASSERT(drawer.mVertexShaderData.VertexBuffer());
			drawer.mVertexShaderData.IndexBuffer() = ShaderResourcesManager::gInstance->Buffer(model->CreateIndexBuffer(iMeshIndex));
			drawer.mVertexShaderData.SetIndexCount(static_cast<unsigned int>(model->Meshes()[iMeshIndex]->Indices().size()));
			drawer.mVertexShaderData.TextureScaleFactor() = textureScaleFactor;

			// Build world matrix
			XMStoreFloat4x4(&drawer.mWorld, worldMatrix);

			// Initialize hull shader data
			for (size_t iTessFactor = 0; iTessFactor < ARRAYSIZE(edgeTesselationFactors); ++iTessFactor) {
				drawer.mHullShaderData.TessellationFactors()[iTessFactor] = edgeTesselationFactors[iTessFactor];
			}
			drawer.mHullShaderData.TessellationFactors()[3] = insideTessellationFactor;

			// Initialize domain shader data
			drawer.mDomainShaderData.DisplacementScale() = displacementScale;
			drawer.mDomainShaderData.DisplacementMapSRV() = displacementSRV;
			drawer.mDomainShaderData.SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

			// Initialize pixel shader data
			drawer.mPixelShaderData.SetMaterial(matId);
			drawer.mPixelShaderData.SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

			if (normalMapSRV) {
				drawer.mPixelShaderData.NormalSRV() = normalMapSRV;
			}

			drawers.push_back(drawer);
		}
	}

	void NormalDisplacementDrawer::Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs, const XMMATRIX& view, const XMMATRIX& proj) {
		const XMMATRIX world = XMLoadFloat4x4(&mWorld);
		XMStoreFloat4x4(&mDomainShaderData.WorldView(), XMMatrixTranspose(world * view));
		XMStoreFloat4x4(&mDomainShaderData.Projection(), XMMatrixTranspose(proj));
		mVertexShaderData.PreDraw(device, context);
		mHullShaderData.PreDraw(device, context);
		mDomainShaderData.PreDraw(device, context);
		mPixelShaderData.PreDraw(device, context, geometryBuffersRTVs);
		mVertexShaderData.DrawIndexed(context);
		mVertexShaderData.PostDraw(context);
		mHullShaderData.PostDraw(context);
		mDomainShaderData.PostDraw(context);
		mPixelShaderData.PostDraw(context);
	}
}