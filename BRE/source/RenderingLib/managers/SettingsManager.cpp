#include "SettingsManager.h"

#include <string>

namespace {
	typedef std::pair<YAML::Node, YAML::Node> NodePair;
}

namespace BRE {
	SettingsManager* SettingsManager::gInstance = nullptr;

	void SettingsManager::LoadFile(const char* file) {
		ASSERT_PTR(file);

		// Compute file hash
		const size_t fileHash = Hash(file);

		// Skip if file was already loaded
		if (mFileMap.find(fileHash) != mFileMap.end()) {
			return;
		}

		// Load yaml file.
		const YAML::Node nodes = YAML::LoadFile(file);
		ASSERT_COND(nodes.IsDefined());
		ASSERT_COND(nodes.IsMap());

		// Create a new entry for current file
		CategoryMap& categoryMap = mFileMap[fileHash];

		// Iterate through each node
		for (const NodePair& seqByCategory : nodes) {
			// Category
			const YAML::Node categoryNode = seqByCategory.first;
			ASSERT_COND(categoryNode.IsDefined());
			const std::string categoryName = categoryNode.as<std::string>();

			// Add an entry for current category name
			const size_t categoryHash = Hash(categoryName.c_str());
			AttributeMap& attrMap = categoryMap[categoryHash];
			ASSERT_COND(attrMap.empty() == true);

			// Get attributes tag.
			// Attributes must be defined and it must be a yaml map
			const YAML::Node attrNodes = seqByCategory.second;
			ASSERT_COND(attrNodes.IsDefined());
			ASSERT_COND(attrNodes.IsMap());

			for (const NodePair& nodePair : attrNodes) {
				// Get attribute name
				const YAML::Node& attrNameNode = nodePair.first;
				ASSERT_COND(attrNameNode.IsScalar());
				const std::string attrName = attrNameNode.as<std::string>();
				const size_t attrHash = Hash(attrName.c_str());

				// Get attribute value
				const YAML::Node& attrValueNode = nodePair.second;

				// Attribute value must be an scalar or a sequence of scalars.
				ASSERT_COND(attrValueNode.IsScalar() || attrValueNode.IsSequence());

				// If attribute value is a scalar
				// we simply store it as a string.
				if (attrValueNode.IsScalar()) {
					const std::string attrValue = attrValueNode.as<std::string>();
					attrMap.insert(std::make_pair(attrHash, attrValue));
				}
				else if (attrValueNode.IsSequence()) {
					// If attribute value is a sequence we store
					// it as a vector of string where each string
					// represents 1 component in the sequence.
					// Each component must be a scalar.
					std::vector<std::string> seqValues;
					for (const YAML::Node& seqNode : attrValueNode) {
						ASSERT_COND(seqNode.IsScalar());
						const std::string seqValue = seqNode.as<std::string>();
						seqValues.push_back(seqValue);
					}
					attrMap.insert(std::make_pair(attrHash, seqValues));
				}
			}
		}
	}
}