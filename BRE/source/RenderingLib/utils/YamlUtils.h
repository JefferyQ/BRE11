#pragma once

#include <string>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>

#include <utils/Assert.h>

namespace BRE {
	class YamlUtils {
	public:
		static bool IsDefined(const YAML::Node& node, const char* key) {
			BRE_ASSERT(key);
			YAML::Node attr = node[key];
			return attr.IsDefined();
		}

		template<typename T>
		static T GetScalar(const YAML::Node& node, const char* key) {
			BRE_ASSERT(key);
			YAML::Node attr = node[key];
			BRE_ASSERT(attr.IsDefined());
			BRE_ASSERT(attr.IsScalar());
			return boost::lexical_cast<T>(attr.as<std::string>());
		}

		template<>
		static std::string GetScalar(const YAML::Node& node, const char* key) {
			BRE_ASSERT(key);
			YAML::Node attr = node[key];
			BRE_ASSERT(attr.IsDefined());
			BRE_ASSERT(attr.IsScalar());
			return attr.as<std::string>();
		}

		template<typename T>
#ifdef _DEBUG
		static void GetSequence(const YAML::Node& node, const char* key, T* const sequence, const size_t numElems) {
#else
		static void GetSequence(const YAML::Node& node, const char* key, T* const sequence, const size_t) {
#endif
			BRE_ASSERT(key);
			BRE_ASSERT(sequence);
			YAML::Node attr = node[key];
			BRE_ASSERT(attr.IsDefined());
			BRE_ASSERT(attr.IsSequence());
			size_t currentNumElems = 0;
			for (const YAML::Node& seqNode : attr) {
				BRE_ASSERT(seqNode.IsScalar());
				BRE_ASSERT(currentNumElems < numElems);
				sequence[currentNumElems] = seqNode.as<T>();
				++currentNumElems;
			}
			BRE_ASSERT(currentNumElems == numElems);
		}

		template<>
#ifdef _DEBUG
		static void GetSequence(const YAML::Node& node, const char* key, float* const sequence, const size_t numElems) {
#else
		static void GetSequence(const YAML::Node& node, const char* key, float* const sequence, const size_t) {
#endif
			BRE_ASSERT(key);
			BRE_ASSERT(sequence);
			YAML::Node attr = node[key];
			BRE_ASSERT(attr.IsDefined());
			BRE_ASSERT(attr.IsSequence());
			size_t currentNumElems = 0;
			for (const YAML::Node& seqNode : attr) {
				BRE_ASSERT(seqNode.IsScalar());
				BRE_ASSERT(currentNumElems < numElems);
				sequence[currentNumElems] = boost::lexical_cast<float>(seqNode.as<std::string>());
				++currentNumElems;
			}
			BRE_ASSERT(currentNumElems == numElems);
		}
	};
}
