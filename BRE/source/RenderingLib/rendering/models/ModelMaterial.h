#pragma once

#include <map>
#include <vector>

struct aiMaterial;

namespace BRE {
	enum TextureType {
		TextureTypeDifffuse = 0,
		TextureTypeSpecularMap,
		TextureTypeAmbient,
		TextureTypeEmissive,
		TextureTypeHeightmap,
		TextureTypeNormalMap,
		TextureTypeSpecularPowerMap,
		TextureTypeDisplacementMap,
		TextureTypeLightMap,
		TextureTypeEnd
	};

	class ModelMaterial {
		friend class Model;

	public:
		ModelMaterial(const Model& model);
		ModelMaterial(const ModelMaterial& rhs) = delete;
		ModelMaterial& operator=(const ModelMaterial& rhs) = delete;
		~ModelMaterial();

		const Model& GetModel() { return mModel; }
		const std::string& Name() const { return mName; }
		const std::map<TextureType, std::vector<std::string>*> Textures() const { return mTextures; }

	private:
		static std::map<TextureType, unsigned int> sTextureTypeMappings;

		ModelMaterial(const Model& model, aiMaterial* material);

		const Model& mModel;
		std::string mName;
		std::map<TextureType, std::vector<std::string>*> mTextures;
	};
}