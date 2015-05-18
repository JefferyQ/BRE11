#pragma once

#include <vector>

namespace BRE {
	class Mesh;
	class ModelMaterial;

	class Model {
	public:
		Model(const char* filename);
		Model(const Model& rhs) = delete;
		Model& operator=(const Model& rhs) = delete;
		~Model();

		const std::string& Filename() const { return mFilename; }
		bool HasMeshes() const { return (mMeshes.size() > 0); }
		bool HasMaterials() const { return (mMaterials.size() > 0); }
		const std::vector<Mesh*>& Meshes() const { return mMeshes; }
		const std::vector<ModelMaterial*>& Materials() const { return mMaterials; }

		size_t Model::CreateIndexBuffer(ID3D11Buffer* *buffer = nullptr) const;

	private:
		std::string mFilename;
		std::vector<Mesh*> mMeshes;
		std::vector<ModelMaterial*> mMaterials;
	};
}
