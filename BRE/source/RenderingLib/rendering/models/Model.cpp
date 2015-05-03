#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

#include <rendering/models/ModelMaterial.h>
#include <rendering/models/Mesh.h>
#include <utils/Assert.h>

namespace BRE {
	Model::Model(const char* filename) {
		ASSERT_PTR(filename);
		Assimp::Importer importer;
#if defined(DEBUG) || defined(_DEBUG)  
		const unsigned int flags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs | aiProcess_MakeLeftHanded;
#else
		const unsigned int flags = aiProcessPreset_TargetRealtime_Fast | aiProcess_FlipUVs | aiProcess_MakeLeftHanded;
#endif
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
				Mesh* mesh = new Mesh(*this, *(scene->mMeshes[i]));
				mMeshes.push_back(mesh);
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
}