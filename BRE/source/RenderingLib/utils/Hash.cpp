#include "Hash.h"

#include <utils/Assert.h>

namespace BRE {
	namespace Utils {
		size_t Hash(const char* str) {
			BRE_ASSERT(str);
			size_t hashValue = 0;
			while (*str) {
				hashValue = hashValue * 101 + static_cast<size_t> (*str++);
			}
			return hashValue;
		}
	}
}