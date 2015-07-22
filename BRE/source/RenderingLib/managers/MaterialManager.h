#pragma once

#include <string>
#include <unordered_map>

struct ID3D11ShaderResourceView;

namespace BRE {
	class MaterialManager {
	public:
		static MaterialManager* gInstance;

		struct InputData {
			std::string mName;
			std::string mNormalTexturePath;
			std::string mBaseColorTexturePath;
			std::string mSmoothnessTexturePath;
			std::string mMetalMaskTexturePath;
			std::string mCurvatureTexturePath;
		};

		struct MaterialData {
			ID3D11ShaderResourceView* mNormalSRV;
			ID3D11ShaderResourceView* mBaseColorSRV;			
			ID3D11ShaderResourceView* mSmoothnessSRV;
			ID3D11ShaderResourceView* mMetalMaskSRV;
			ID3D11ShaderResourceView* mCurvatureSRV;
		};

		void LoadMaterials(const char* materialFile);
		size_t AddMaterial(const InputData& data, MaterialData* material = nullptr);
		void GetMaterial(const size_t id, MaterialData& material) const;

	private:
		struct MaterialDataId {
			size_t mNormal;
			size_t mBaseColor;
			size_t mSmoothness;
			size_t mMetalMask;
			size_t mCurvature;
		};
		
		typedef std::unordered_map<size_t, MaterialDataId> MaterialDataIdById;
		MaterialDataIdById mMaterialDataIdById;

	};
}
