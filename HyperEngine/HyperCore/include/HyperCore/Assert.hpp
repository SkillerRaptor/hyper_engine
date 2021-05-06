#pragma once

#include <HyperCore/Log.hpp>
#include <HyperCore/Prerequisites.hpp>

#include <cassert>

#if HYPERENGINE_DEBUG
	#define HYPERENGINE_INTERNAL_ASSERT_IMPLEMENTATION(check, msg, ...) \
		do                                                              \
		{                                                               \
			if (!(check))                                               \
			{                                                           \
				HYPERENGINE_CORE_FATAL(msg, __VA_ARGS__);               \
				assert(check);                                          \
			}                                                           \
		} while (0)

	#define HYPERENGINE_INTERNAL_ASSERT_WITH_MSG(check, ...) HYPERENGINE_INTERNAL_ASSERT_IMPLEMENTATION(check, "Assertion failed: {}", __VA_ARGS__)
	#define HYPERENGINE_INTERNAL_ASSERT_NO_MSG(check) HYPERENGINE_INTERNAL_ASSERT_IMPLEMENTATION(check, "Assertion '{}' failed in {} at line {}", HYPERENGINE_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)
	#define HYPERENGINE_INTERNAL_ASSERT_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define HYPERENGINE_INTERNAL_ASSERT_MACRO(...) HYPERENGINE_EXPAND_MACRO(HYPERENGINE_INTERNAL_ASSERT_MACRO_NAME(__VA_ARGS__, HYPERENGINE_INTERNAL_ASSERT_WITH_MSG, HYPERENGINE_INTERNAL_ASSERT_NO_MSG))

	#define HYPERENGINE_ASSERT(...) HYPERENGINE_EXPAND_MACRO(HYPERENGINE_INTERNAL_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__))
#else
	#define HYPERENGINE_ASSERT(...)
#endif
