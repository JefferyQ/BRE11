#pragma once

#include <unordered_map>

namespace BRE {
	class Model;

	class ModelManager {
	public:
		static ModelManager* gInstance;
		ModelManager() {}
		~ModelManager();
		ModelManager(const ModelManager&) = delete;
		const ModelManager& operator=(const ModelManager&) = delete;

		size_t LoadModel(const char* modelPath, const Model* *model = nullptr);
		const Model* GetModel(const size_t id);

	private:
		typedef std::unordered_map<size_t, Model*> ModelById;
		ModelById mModelById;
	};


}
