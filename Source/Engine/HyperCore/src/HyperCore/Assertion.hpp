/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Logger.hpp>
#include <HyperCore/Utilities/Prerequisites.hpp>
#include <HyperCore/Utilities/SourceLocation.hpp>

#if HYPERENGINE_DEBUG
#	define HYPERENGINE_ASSERT(expression)                                                            \
		do                                                                                            \
		{                                                                                             \
			if (!(expression))                                                                        \
			{                                                                                         \
				HyperCore::CSourceLocation current_location = HyperCore::CSourceLocation::current();  \
				HyperCore::CLogger::fatal("Assertion failed: {}", HYPERENGINE_STRINGIFY(expression)); \
				HyperCore::CLogger::fatal(                                                            \
					"{}:{} in {}",                                                                    \
					current_location.file_name(),                                                     \
					current_location.line(),                                                          \
					current_location.function_name());                                                \
				std::abort();                                                                         \
			}                                                                                         \
		} while (0)
#	define HYPERENGINE_ASSERT_WITH_MESSAGE(expression, message)                                     \
		do                                                                                           \
		{                                                                                            \
			if (!(expression))                                                                       \
			{                                                                                        \
				HyperCore::CSourceLocation current_location = HyperCore::CSourceLocation::current(); \
				HyperCore::CLogger::fatal("Assertion failed: {}", message);                          \
				HyperCore::CLogger::fatal(                                                           \
					"{}:{} in {}",                                                                   \
					current_location.file_name(),                                                    \
					current_location.line(),                                                         \
					current_location.function_name());                                               \
				std::abort();                                                                        \
			}                                                                                        \
		} while (0)
#	define HYPERENGINE_ASSERT_NOT_REACHED() HYPERENGINE_ASSERT(false)
#else
#	define HYPERENGINE_ASSERT(expression)
#	define HYPERENGINE_ASSERT_WITH_MESSAGE(expression, message)
#	define HYPERENGINE_ASSERT_NOT_REACHED() std::abort()
#endif
