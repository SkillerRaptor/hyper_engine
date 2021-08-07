/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Logger.hpp"
#include "HyperCore/Prerequisites.hpp"
#include "HyperCore/SourceLocation.hpp"

#if HYPERENGINE_DEBUG
#	define HYPERENGINE_ASSERT(expression)                                                           \
		do                                                                                           \
		{                                                                                            \
			if (!(expression))                                                                       \
			{                                                                                        \
				HyperCore::SourceLocation current_location = HyperCore::SourceLocation::current();   \
				HyperCore::Logger::fatal("Assertion failed: {}", HYPERENGINE_STRINGIFY(expression)); \
				HyperCore::Logger::fatal(                                                            \
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
#	define HYPERENGINE_ASSERT_NOT_REACHED() std::abort()
#endif
