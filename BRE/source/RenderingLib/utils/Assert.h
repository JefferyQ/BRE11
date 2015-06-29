#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>

#if defined(DEBUG) || defined(_DEBUG)
#define BRE_ASSERT(condition) \
	assert(condition);
#else
#define BRE_ASSERT(condition);
#endif

#if defined(_DEBUG) || defined(DEBUG)
#ifndef ASSERT_HR
#define ASSERT_HR(x){																\
		const HRESULT hr = (x);														\
		if(FAILED(hr)){															\
			std::cout << "An error occured on line" << (DWORD)__LINE__ << " in the file " << __FILE__ << std::endl; \
			std::cout << hr << std::endl; \
			abort(); \
		}																		\
	}
#endif
#else
#ifndef ASSERT_HR
#define ASSERT_HR(x) (x)
#endif
#endif
