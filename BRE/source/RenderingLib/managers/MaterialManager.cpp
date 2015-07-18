#include "MaterialManager.h"

#include <managers/ShaderResourcesManager.h>

#include <utils/Assert.h>
#include <utils/Hash.h>
#include <utils/YamlUtils.h>

namespace BRE {
	MaterialManager* MaterialManager::gInstance = nullptr;

	void MaterialManager::LoadMaterials(const char* materialFile) {
		BRE_ASSERT(materialFile);
		const YAML::Node yamlFile = YAML::LoadFile(materialFile);
		BRE_ASSERT(yamlFile.IsDefined());
		const YAML::Node nodes = yamlFile["materials"];
		BRE_ASSERT(nodes.IsDefined());
		BRE_ASSERT(nodes.IsSequence());
		for (const YAML::Node& node : nodes) {
			BRE_ASSERT(node.IsDefined());
			BRE_ASSERT(node.IsMap());
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
		const size_t id = Utils::Hash(data.mName.c_str());
		BRE_ASSERT(mMaterialDataIdById.find(id) == mMaterialDataIdById.end());		
		MaterialDataId& newMaterialId = mMaterialDataIdById[id];
		newMaterialId.mNormal = ShaderResourcesManager::gInstance->AddTextureFromFileSRV(data.mNormalTexturePath.c_str(), (material) ? &material->mNormalSRV : nullptr);
		newMaterialId.mBaseColor = ShaderResourcesManager::gInstance->AddTextureFromFileSRV(data.mBaseColorTexturePath.c_str(), (material) ? &material->mBaseColorSRV : nullptr, true);
		newMaterialId.mSmoothness = ShaderResourcesManager::gInstance->AddTextureFromFileSRV(data.mSmoothnessTexturePath.c_str(), (material) ? &material->mSmoothnessSRV : nullptr);
		newMaterialId.mMetalMask = ShaderResourcesManager::gInstance->AddTextureFromFileSRV(data.mMetalMaskTexturePath.c_str(), (material) ? &material->mMetalMaskSRV : nullptr);
		newMaterialId.mReflectance = ShaderResourcesManager::gInstance->AddTextureFromFileSRV(data.mReflectanceTexturePath.c_str(), (material) ? &material->mReflectanceSRV : nullptr, true);
		return id;
	}

	void MaterialManager::GetMaterial(const size_t id, MaterialManager::MaterialData& material) const {
		auto findIt = mMaterialDataIdById.find(id);
		BRE_ASSERT(findIt != mMaterialDataIdById.end());
		material.mNormalSRV = ShaderResourcesManager::gInstance->ShaderResourceView(findIt->second.mNormal);
		BRE_ASSERT(material.mNormalSRV);
		material.mBaseColorSRV = ShaderResourcesManager::gInstance->ShaderResourceView(findIt->second.mBaseColor);
		BRE_ASSERT(material.mBaseColorSRV);
		material.mSmoothnessSRV = ShaderResourcesManager::gInstance->ShaderResourceView(findIt->second.mSmoothness);
		BRE_ASSERT(material.mSmoothnessSRV);
		material.mMetalMaskSRV = ShaderResourcesManager::gInstance->ShaderResourceView(findIt->second.mMetalMask);
		BRE_ASSERT(material.mMetalMaskSRV);
		material.mReflectanceSRV = ShaderResourcesManager::gInstance->ShaderResourceView(findIt->second.mReflectance);
		BRE_ASSERT(material.mReflectanceSRV);
	}
}