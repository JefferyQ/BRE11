#include "Hash.h"

#include <utils/Assert.h>

namespace BRE {
	size_t Hash(const char* str) {
		ASSERT_PTR(str);
		size_t hashValue = 0;
		while (*str) {
			hashValue = hashValue * 101 + static_cast<size_t> (*str++);
		}
		return hashValue;
	}
}