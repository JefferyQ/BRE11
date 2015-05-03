#include "ModelMaterial.h"

#include <assimp/scene.h>

#include <utils/Assert.h>
#include <utils/Memory.h>
#include <utils/Utility.h>

namespace {
	void InitializeTextureTypeMappings(std::map<BRE::TextureType, unsigned int> textureTypeMappings) {
		if (textureTypeMappings.size() != BRE::TextureTypeEnd) {
			textureTypeMappings[BRE::TextureTypeDifffuse] = aiTextureType_DIFFUSE;
			textureTypeMappings[BRE::TextureTypeSpecularMap] = aiTextureType_SPECULAR;
			textureTypeMappings[BRE::TextureTypeAmbient] = aiTextureType_AMBIENT;
			textureTypeMappings[BRE::TextureTypeHeightmap] = aiTextureType_HEIGHT;
			textureTypeMappings[BRE::TextureTypeNormalMap] = aiTextureType_NORMALS;
			textureTypeMappings[BRE::TextureTypeSpecularPowerMap] = aiTextureType_SHININESS;
			textureTypeMappings[BRE::TextureTypeDisplacementMap] = aiTextureType_DISPLACEMENT;
			textureTypeMappings[BRE::TextureTypeLightMap] = aiTextureType_LIGHTMAP;
		}
	}
}

namespace BRE {
	std::map<TextureType, unsigned int> ModelMaterial::sTextureTypeMappings;

	ModelMaterial::ModelMaterial(const Model& model)
		: mModel(model)
	{
		InitializeTextureTypeMappings(sTextureTypeMappings);
	}

	ModelMaterial::ModelMaterial(const Model& model, aiMaterial* material)
		: mModel(model)
	{
		InitializeTextureTypeMappings(sTextureTypeMappings);

		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		mName = name.C_Str();

		for (TextureType textureType = static_cast<TextureType>(0); textureType < TextureTypeEnd; textureType = static_cast<TextureType>(textureType + 1)) {
			const aiTextureType mappedTextureType = static_cast<aiTextureType>(sTextureTypeMappings[textureType]);

			const unsigned int textureCount = material->GetTextureCount(mappedTextureType);
			if (textureCount > 0) {
				std::vector<std::string>* textures = new std::vector<std::string>();
				mTextures.insert(std::make_pair(textureType, textures));
				textures->reserve(textureCount);

				for (unsigned int textureIndex = 0; textureIndex < textureCount; ++textureIndex) {
					aiString path;
					if (material->GetTexture(mappedTextureType, textureIndex, &path) == AI_SUCCESS) {
						textures->push_back(path.C_Str());
					}
				}
			}
		}
	}

	ModelMaterial::~ModelMaterial() {
		for (std::pair<TextureType, std::vector<std::string>*> textures : mTextures) {
			DELETE_OBJECT(textures.second);
		}
	}
}