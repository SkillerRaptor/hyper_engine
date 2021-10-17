/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/Platform.hpp"
#include "HyperEngine/Core/Prerequisites.hpp"
#include "HyperEngine/Core/Logger.hpp"
#include "HyperEngine/Core/SourceLocation.hpp"

#include <cstdlib>

#ifdef HYPERENGINE_DEBUG
#	define HYPERENGINE_ASSERT(condition)                                                                      \
		do                                                                                                     \
		{                                                                                                      \
			if (!(condition))                                                                                  \
			{                                                                                                  \
				const HyperEngine::CSourceLocation current_location = HyperEngine::CSourceLocation::current(); \
				HyperEngine::CLogger::fatal("Assertion failed: {}", HYPERENGINE_STRINGIFY(condition));         \
				HyperEngine::CLogger::fatal(                                                                   \
					"{}:{} in {}",                                                                             \
					current_location.file_name(),                                                              \
					current_location.line(),                                                                   \
					current_location.function_name());                                                         \
				HYPERENGINE_DEBUG_BREAK();                                                                     \
			}                                                                                                  \
		} while (false)
#	define HYPERENGINE_ASSERT_NOT_REACHED() HYPERENGINE_ASSERT(false)
#elif
#	define HYPERENGINE_ASSERT(condition)
#	define HYPERENGINE_ASSERT_NOT_REACHED() std::abort()
#endif

#define HYPERENGINE_ASSERT_IS_EQUAL(expected, actual) HYPERENGINE_ASSERT((expected) == (actual))
#define HYPERENGINE_ASSERT_IS_NOT_EQUAL(expected, actual) HYPERENGINE_ASSERT((expected) != (actual))
#define HYPERENGINE_ASSERT_IS_TRUE(condition) HYPERENGINE_ASSERT((condition))
#define HYPERENGINE_ASSERT_IS_FALSE(condition) HYPERENGINE_ASSERT(!(condition))
#define HYPERENGINE_ASSERT_IS_NULL(ptr) HYPERENGINE_ASSERT((ptr) == nullptr)
#define HYPERENGINE_ASSERT_IS_NOT_NULL(ptr) HYPERENGINE_ASSERT((ptr) != nullptr)
