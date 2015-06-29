#include "ModelManager.h"

#include <rendering/models/Model.h>
#include <utils/Assert.h>
#include <utils/Hash.h>

namespace BRE {
	ModelManager* ModelManager::gInstance = nullptr;

	ModelManager::~ModelManager() {
		for (auto elem : mModelById) {
			delete elem.second;
		}
	}

	size_t ModelManager::LoadModel(const char* modelPath, const Model* *model) {
		BRE_ASSERT(modelPath);
		const size_t id = Utils::Hash(modelPath);
		ModelById::const_iterator it = mModelById.find(id);
		if (it == mModelById.end()) {
			Model* elem = new Model(modelPath);
			if (model) *model = elem;
			mModelById[id] = new Model(modelPath);
		}
		else {
			if (model) *model = it->second;
		}

		return id;
	}

	const Model* ModelManager::GetModel(const size_t id) {
		ModelById::const_iterator it = mModelById.find(id);
		return (it != mModelById.end()) ? it->second : nullptr;
	}
}