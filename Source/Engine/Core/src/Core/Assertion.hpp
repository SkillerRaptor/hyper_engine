#pragma once

#include "Logger.hpp"
#include "Utilities/Macros.hpp"
#include "Utilities/SourceLocation.hpp"

#if HE_DEBUG
#define HE_VERIFY(expression) \
	do \
    { \
        if (!(expression)) \
        { \
            Core::SourceLocation location = Core::SourceLocation::current(); \
            Core::Logger::fatal("Assertion failed: {}", HE_STRINGIFY(expression));  \
            Core::Logger::fatal("{}:{} in {}", location.file_name(), location.line(), location.function_name()); \
            std::abort(); \
		} \
    } while (0)
#define HE_VERIFY_NOT_REACHED() HE_VERIFY(false)
#else
#define HE_VERIFY(expression)
#define HE_VERIFY_NOT_REACHED() std::abort();
#endif
