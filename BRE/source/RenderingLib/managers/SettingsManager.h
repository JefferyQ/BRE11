#pragma once

//////////////////////////////////////////////////////////////////////////
//
// The purpose of this class is to centralize the management of
// different settings.
// You can read settings from yaml files and ask for their values.
//
//////////////////////////////////////////////////////////////////////////

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

#include <utils/Assert.h>
#include <utils/Hash.h>

namespace BRE {
	class SettingsManager {
		// We use hashed keys
		// For example: texturePath -> filename.dds
		typedef std::pair<size_t, boost::any> Attribute;
		typedef std::unordered_map<size_t, boost::any> AttributeMap;

		// For example: Resources -> { texturePath1 -> filename1.dds, texturePath2 -> filename2.dds }
		typedef std::pair<size_t, AttributeMap> Category;
		typedef std::unordered_map<size_t, AttributeMap> CategoryMap;

		// For example: application.yml -> {Resources, Shaders}
		typedef std::pair<size_t, CategoryMap> File;
		typedef std::unordered_map<size_t, CategoryMap> FileMap;

	public:
		static SettingsManager* gInstance;

		void LoadFile(const char* file);

		template<typename T>
		T GetScalarAttribute(const char* file, const char* category, const char* attribute) const;

		template<typename T>
		std::vector<T> GetSequenceAttribute(const char* file, const char* category, const char* attribute) const;

	private:
		FileMap mFileMap;
	};

	template<typename T>
	T SettingsManager::GetScalarAttribute(const char* file, const char* category, const char* attribute) const {
		ASSERT_PTR(file);
		ASSERT_PTR(category);
		ASSERT_PTR(attribute);

		// Get hashes
		const size_t fileHash = Hash(file);
		const size_t categoryHash = Hash(category);
		const size_t attrHash = Hash(attribute);

		// Get file
		FileMap::const_iterator fileIt = mFileMap.find(fileHash);
		ASSERT_COND(fileIt != mFileMap.end());

		// Get category
		const CategoryMap& categoryMap = fileIt->second;
		CategoryMap::const_iterator categoryIt = categoryMap.find(categoryHash);
		ASSERT_COND(categoryIt != categoryMap.end());

		// Get attribute
		const AttributeMap& attrMap = categoryIt->second;
		AttributeMap::const_iterator attrIt = attrMap.find(attrHash);
		ASSERT_COND(attrIt != attrMap.end());

		// Get string stored in boost::any
		boost::any attrAny = attrIt->second;
		const std::string attrStr = boost::any_cast<std::string>(attrAny);

		// Cast string to scalar
		return boost::lexical_cast<T>(attrStr);
	}

	template<typename T>
	std::vector<T> SettingsManager::GetSequenceAttribute(const char* file, const char* category, const char* attribute) const {
		ASSERT_PTR(file);
		ASSERT_PTR(category);
		ASSERT_PTR(attribute);

		// Get hashes
		const size_t fileHash = Hash(file);
		const size_t categoryHash = Hash(category);
		const size_t attrHash = Hash(attribute);

		// Get file
		FileMap::const_iterator fileIt = mFileMap.find(fileHash);
		ASSERT_COND(fileIt != mFileMap.end());

		// Get category
		const CategoryMap& categoryMap = fileIt->second;
		CategoryMap::const_iterator categoryIt = categoryMap.find(categoryHash);
		ASSERT_COND(categoryIt != categoryMap.end());

		// Get attribute
		const AttributeMap& attrMap = categoryIt->second;
		AttributeMap::const_iterator attrIt = attrMap.find(attrHash);
		ASSERT_COND(attrIt != attrMap.end());

		// Get std::vector<string> stored in boost::any
		boost::any attrAny = attrIt->second;
		const std::vector<std::string> attrSeq = boost::any_cast<std::vector<std::string>>(attrAny);

		std::vector<T> sequence;
		for (const std::string& str : attrSeq) {
			sequence.push_back(boost::lexical_cast<T>(str));
		}

		return sequence;
	}
}