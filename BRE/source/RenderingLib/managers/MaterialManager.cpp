#include "MaterialManager.h"

#include <managers/ShaderResourcesManager.h>

#include <utils/Assert.h>
#include <utils/Hash.h>
#include <utils/YamlUtils.h>

namespace BRE {
	MaterialManager* MaterialManager::gInstance = nullptr;

	void MaterialManager::LoadMaterials(const char* materialFile) {
		ASSERT_PTR(materialFile);

		const YAML::Node yamlFile = YAML::LoadFile(materialFile);
		ASSERT_COND(yamlFile.IsDefined());

		const YAML::Node nodes = yamlFile["materials"];
		ASSERT_COND(nodes.IsDefined());
		ASSERT_COND(nodes.IsSequence());

		for (const YAML::Node& node : nodes) {
			ASSERT_COND(node.IsDefined());
			ASSERT_COND(node.IsMap());

			InputData data;
			data.mName = YamlUtils::GetScalar<std::string>(node, "name");
			data.mNormalTexturePath = YamlUtils::GetScalar<std::string>(node, "normal");
			data.mBaseColorTexturePath = YamlUtils::GetScalar<std::string>(node, "baseColor");
			data.mSmoothnessTexturePath = YamlUtils::GetScalar<std::string>(node, "smoothness");
			data.mMetalMaskTexturePath = YamlUtils::GetScalar<std::string>(node, "metalMask");
			data.mReflectanceTexturePath = YamlUtils::GetScalar<std::string>(node, "reflectance");
			AddMaterial(data);
		}
	}

	size_t MaterialManager::AddMaterial(const InputData& data, MaterialData* material) {
		const size_t id = Hash(data.mName.c_str());
		ASSERT_COND(mMaterialDataIdById.find(id) == mMaterialDataIdById.end());
		
		MaterialDataId& newMaterialId = mMaterialDataIdById[id];
		newMaterialId.mNormal = ShaderResourcesManager::gInstance->AddTextureFromFileSRV(data.mNormalTexturePath.c_str(), (material) ? &material->mNormalSRV : nullptr);
		newMaterialId.mBaseColor = ShaderResourcesManager::gInstance->AddTextureFromFileSRV(data.mBaseColorTexturePath.c_str(), (material) ? &material->mBaseColorSRV : nullptr);
		newMaterialId.mSmoothness = ShaderResourcesManager::gInstance->AddTextureFromFileSRV(data.mSmoothnessTexturePath.c_str(), (material) ? &material->mSmoothnessSRV : nullptr);
		newMaterialId.mMetalMask = ShaderResourcesManager::gInstance->AddTextureFromFileSRV(data.mMetalMaskTexturePath.c_str(), (material) ? &material->mMetalMaskSRV : nullptr);
		newMaterialId.mReflectance = ShaderResourcesManager::gInstance->AddTextureFromFileSRV(data.mReflectanceTexturePath.c_str(), (material) ? &material->mReflectanceSRV : nullptr);
		return id;
	}

	void MaterialManager::GetMaterial(const size_t id, MaterialManager::MaterialData& material) const {
		auto findIt = mMaterialDataIdById.find(id);
		ASSERT_COND(findIt != mMaterialDataIdById.end());

		material.mNormalSRV = ShaderResourcesManager::gInstance->ShaderResourceView(findIt->second.mNormal);
		ASSERT_PTR(material.mNormalSRV);
		material.mBaseColorSRV = ShaderResourcesManager::gInstance->ShaderResourceView(findIt->second.mBaseColor);
		ASSERT_PTR(material.mBaseColorSRV);
		material.mSmoothnessSRV = ShaderResourcesManager::gInstance->ShaderResourceView(findIt->second.mSmoothness);
		ASSERT_PTR(material.mSmoothnessSRV);
		material.mMetalMaskSRV = ShaderResourcesManager::gInstance->ShaderResourceView(findIt->second.mMetalMask);
		ASSERT_PTR(material.mMetalMaskSRV);
		material.mReflectanceSRV = ShaderResourcesManager::gInstance->ShaderResourceView(findIt->second.mReflectance);
		ASSERT_PTR(material.mReflectanceSRV);
	}
}