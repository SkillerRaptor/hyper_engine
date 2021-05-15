#pragma once

#include <HyperCore/Logger.hpp>
#include <HyperCore/Prerequisites.hpp>

#include <cstdlib>

#if HYPERENGINE_DEBUG
	#define HYPERENGINE_INTERNAL_ASSERT_IMPLEMENTATION(check, msg, ...) \
		do                                                              \
		{                                                               \
			if (!(check))                                               \
			{                                                           \
				HyperCore::Logger::Fatal(msg, __VA_ARGS__);             \
				std::abort();                                           \
			}                                                           \
		} while (0)

	#define HYPERENGINE_INTERNAL_ASSERT_WITH_MSG(check, ...) HYPERENGINE_INTERNAL_ASSERT_IMPLEMENTATION(check, "Assertion failed: {}", __VA_ARGS__)
	#define HYPERENGINE_INTERNAL_ASSERT_NO_MSG(check) HYPERENGINE_INTERNAL_ASSERT_IMPLEMENTATION(check, "Assertion '{}' failed in {} at line {}", HYPERENGINE_STRINGIFY_MACRO(check), __FILE__, __LINE__)
	#define HYPERENGINE_INTERNAL_ASSERT_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define HYPERENGINE_INTERNAL_ASSERT_MACRO(...) HYPERENGINE_EXPAND_MACRO(HYPERENGINE_INTERNAL_ASSERT_MACRO_NAME(__VA_ARGS__, HYPERENGINE_INTERNAL_ASSERT_WITH_MSG, HYPERENGINE_INTERNAL_ASSERT_NO_MSG))

	#define HYPERENGINE_ASSERT(...) HYPERENGINE_EXPAND_MACRO(HYPERENGINE_INTERNAL_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__))
#else
	#define HYPERENGINE_ASSERT(...)
#endif
