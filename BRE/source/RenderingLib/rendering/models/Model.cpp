#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <d3d11_1.h>
#include <iostream>
#include <sstream>

#include <managers/ShaderResourcesManager.h>
#include <rendering/models/ModelMaterial.h>
#include <rendering/models/Mesh.h>
#include <utils/Assert.h>
#include <utils/Hash.h>
#include <utils/Utility.h>

namespace BRE {
	Model::Model(const char* filename) 
		: mFilename(filename)
	{
		ASSERT_PTR(filename);
		Assimp::Importer importer;
		const unsigned int flags = aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded;
		const aiScene* scene = importer.ReadFile(filename, flags);
		if (!scene) {
			const char* errorMsg = importer.GetErrorString();
			std::cerr << errorMsg << std::endl;
			ASSERT_PTR(scene);
		}
		
		if (scene->HasMaterials()) {
			for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
				mMaterials.push_back(new ModelMaterial(*this, scene->mMaterials[i]));
			}
		}
		if (scene->HasMeshes()) {
			for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
				aiMesh* mesh = scene->mMeshes[i];
				mMeshes.push_back(new Mesh(*this, *(mesh)));
			}
		}
	}

	Model::~Model() {
		for (Mesh* mesh : mMeshes) {
			delete mesh;
		}
		for (ModelMaterial* material : mMaterials) {
			delete material;
		}
	}

	size_t Model::CreateIndexBuffer(const size_t meshIndex, ID3D11Buffer* *buffer) const {
		ASSERT_COND(meshIndex < mMeshes.size());

		// Check if there is already a buffer for current model
		std::stringstream stream;
		stream << meshIndex;
		const std::string bufferName = mFilename + std::string("_indexBuffer_") + stream.str();
		const size_t bufferId = Hash(bufferName.c_str());
		ID3D11Buffer* elem = ShaderResourcesManager::gInstance->Buffer(bufferId);
		if (elem) {
			if (buffer) *buffer = elem;
			return bufferId;
		}

		// Create buffer
		BRE::Mesh& mesh = *Meshes()[meshIndex];
		ASSERT_COND(!mesh.Indices().empty());
		std::vector<unsigned int> indices;
		indices.insert(indices.end(), mesh.Indices().begin(), mesh.Indices().end());
		const unsigned int bufferSize = static_cast<unsigned int> (indices.size() * sizeof(unsigned int));
		Utility::CreateInitializedBuffer(bufferName.c_str(), &indices[0], bufferSize, D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, buffer);

		return bufferId;
	}
}