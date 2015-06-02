#pragma once

//////////////////////////////////////////////////////////////////////////
//
// Assertions
//
//////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>

// Asserts for pointers (pointer != nullptr)
// No effect in Release mode.
#if defined(DEBUG) || defined(_DEBUG)
#define ASSERT_PTR(pointer) \
	if ((pointer) == nullptr) { \
		abort(); \
	}
#else
#define ASSERT_PTR(pointer) ;
#endif

// Asserts (condition == true)
// No effect in Release mode.
#if defined(DEBUG) || defined(_DEBUG)
#define ASSERT_COND(condition) \
	if ((condition) == false) { \
		abort(); \
	}
#else
#define ASSERT_COND(condition) ;
#endif

// Asserts (value == expectedValue)
// No effect in Release mode.
#if defined(DEBUG) || defined(_DEBUG)
#define ASSERT_EQ(value, expectedValue) \
	if ((value) != (expectedValue)) { \
		abort(); \
	}
#else
#define ASSERT_EQ(value, expectedValue) ;
#endif

// Useful to check function return value
// because it keeps function call in Debug and Release modes
#if defined(DEBUG) || defined(_DEBUG)
#define ASSERT_RET(function, expectedValue) \
	if ((function) != (expectedValue)) { \
		abort(); \
	}
#else
#define ASSERT_RET(function, expectedValue) (function)
#endif

// Assert to check HRESULT's
// It keeps function call in Debug and Release modes
#if defined(_DEBUG) || defined(DEBUG)
#ifndef ASSERT_HR
#define ASSERT_HR(x){																\
		HRESULT hr = (x);														\
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
