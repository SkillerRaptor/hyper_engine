#pragma once

#include <core/logger.hpp>
#include <core/macro_utilities.hpp>
#include <core/stl/source_location.hpp>

#include <filesystem>
#include <string>

namespace core
{
#if HYPERENGINE_DEBUG
#   define HYPERENGINE_ASSERT(condition) \
        do \
		{ \
			if (!(condition)) \
			{ \
				core::source_location location{ core::source_location::current() }; \
				core::logger::fatal( \
					"Assertion failed: {}, at {}:{}", \
					HYPERENGINE_STRINGIFY_MACRO(condition), \
					std::filesystem::path(location.file_name()).filename().string(), \
					location.line()); \
				std::abort(); \
			} \
		} while (0)
#else
#   define HYPERENGINE_ASSERT()
#endif
}
