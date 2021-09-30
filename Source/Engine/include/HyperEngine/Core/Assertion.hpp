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
#	define HYPERENGINE_ASSERT_IMPLEMENTATION(condition, string_condition, ...)                            \
		do                                                                                                 \
		{                                                                                                  \
			if (!(condition))                                                                              \
			{                                                                                              \
				HyperEngine::CSourceLocation current_location = HyperEngine::CSourceLocation::current();   \
				HyperEngine::CLogger::fatal("Assertion failed: {} | {}", string_condition, ##__VA_ARGS__); \
				HyperEngine::CLogger::fatal(                                                               \
					"{}:{} in {}",                                                                         \
					current_location.file_name(),                                                          \
					current_location.line(),                                                               \
					current_location.function_name());                                                     \
				HYPERENGINE_DEBUG_BREAK();                                                                 \
			}                                                                                              \
		} while (false)
#	define HYPERENGINE_ASSERT(condition, ...) HYPERENGINE_ASSERT_IMPLEMENTATION(condition, HYPERENGINE_STRINGIFY(condition), ##__VA_ARGS__)
#	define HYPERENGINE_ASSERT_NOT_REACHED() HYPERENGINE_ASSERT(false)
#elif
#	define HYPERENGINE_ASSERT(condition, ...)
#	define HYPERENGINE_ASSERT_NOT_REACHED() std::abort()
#endif

#define HYPERENGINE_ASSERT_IS_EQUAL(expected, actual, ...) HYPERENGINE_ASSERT(expected == actual, ##__VA_ARGS__)
#define HYPERENGINE_ASSERT_IS_NOT_EQUAL(expected, actual, ...) HYPERENGINE_ASSERT(expected != actual, ##__VA_ARGS__)
#define HYPERENGINE_ASSERT_IS_TRUE(condition, ...) HYPERENGINE_ASSERT(condition, ##__VA_ARGS__)
#define HYPERENGINE_ASSERT_IS_FALSE(condition, ...) HYPERENGINE_ASSERT(!condition, ##__VA_ARGS__)
#define HYPERENGINE_ASSERT_IS_NULL(ptr, ...) HYPERENGINE_ASSERT(ptr == nullptr, ##__VA_ARGS__)
#define HYPERENGINE_ASSERT_IS_NOT_NULL(ptr, ...) HYPERENGINE_ASSERT(ptr != nullptr, ##__VA_ARGS__)
