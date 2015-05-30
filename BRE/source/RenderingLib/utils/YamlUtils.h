#pragma once

#include <string>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>

#include <utils/Assert.h>

namespace BRE {
	class YamlUtils {
	public:
		template<typename T>
		static T GetScalar(const YAML::Node& node, const char* key) {
			ASSERT_PTR(key);

			YAML::Node attr = node[key];
			ASSERT_COND(attr.IsDefined());
			ASSERT_COND(attr.IsScalar());
			return boost::lexical_cast<T>(attr.as<std::string>());
		}

		template<>
		static std::string GetScalar(const YAML::Node& node, const char* key) {
			ASSERT_PTR(key);

			YAML::Node attr = node[key];
			ASSERT_COND(attr.IsDefined());
			ASSERT_COND(attr.IsScalar());
			return attr.as<std::string>();
		}

		template<typename T>
#ifdef _DEBUG
		static void GetSequence(const YAML::Node& node, const char* key, T* const sequence, const size_t numElems) {
#else
		static void GetSequence(const YAML::Node& node, const char* key, T* const sequence, const size_t) {
#endif
			ASSERT_PTR(key);
			ASSERT_PTR(sequence);

			YAML::Node attr = node[key];
			ASSERT_COND(attr.IsDefined());
			ASSERT_COND(attr.IsSequence());
			size_t currentNumElems = 0;
			for (const YAML::Node& seqNode : attr) {
				ASSERT_COND(seqNode.IsScalar());
				ASSERT_COND(currentNumElems < numElems);
				sequence[currentNumElems] = seqNode.as<T>();
				++currentNumElems;
			}
			ASSERT_COND(currentNumElems == numElems);
		}

		template<>
#ifdef _DEBUG
		static void GetSequence(const YAML::Node& node, const char* key, float* const sequence, const size_t numElems) {
#else
		static void GetSequence(const YAML::Node& node, const char* key, float* const sequence, const size_t) {
#endif
			ASSERT_PTR(key);
			ASSERT_PTR(sequence);

			YAML::Node attr = node[key];
			ASSERT_COND(attr.IsDefined());
			ASSERT_COND(attr.IsSequence());
			size_t currentNumElems = 0;
			for (const YAML::Node& seqNode : attr) {
				ASSERT_COND(seqNode.IsScalar());
				ASSERT_COND(currentNumElems < numElems);
				sequence[currentNumElems] = boost::lexical_cast<float>(seqNode.as<std::string>());
				++currentNumElems;
			}
			ASSERT_COND(currentNumElems == numElems);
		}
	};
}
