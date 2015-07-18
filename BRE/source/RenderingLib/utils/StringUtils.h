#pragma once

#include <string>

namespace BRE {
	namespace Utils {
		void ToWideString(const std::string& source, std::wstring& dest);
		std::wstring ToWideString(const std::string& source);
		std::string ToString(const std::wstring& source);
	};
}