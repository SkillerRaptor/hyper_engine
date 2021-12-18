/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if defined(__clang__)
#	define HYPERENGINE_COMPILER_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
#	define HYPERENGINE_COMPILER_GCC 1
#elif defined(_MSC_VER)
#	define HYPERENGINE_COMPILER_MSVC 1
#else
#	error Unsupported compiler was used
#endif

#if HYPERENGINE_COMPILER_CLANG || HYPERENGINE_COMPILER_GCC
#	define HYPERENGINE_BUILTIN_UNREACHABLE() __builtin_unreachable()
#elif HYPERENGINE_COMPILER_MSVC
#	define HYPERENGINE_BUILTIN_UNREACHABLE() __assume(false)
#endif

// clang-format off
#if HYPERENGINE_COMPILER_CLANG
#	define HYPERENGINE_DISABLE_WARNINGS() \
	_Pragma("clang diagnostic push")       \
	_Pragma("clang diagnostic ignored \"-Weverything\"")
#	define HYPERENGINE_RESTORE_WARNINGS() _Pragma("clang diagnostic pop")
#elif HYPERENGINE_COMPILER_GCC
#	define HYPERENGINE_DISABLE_WARNINGS()        \
	_Pragma("GCC diagnostic push")                \
	_Pragma("GCC diagnostic ignored \"-Wall\"")   \
	_Pragma("GCC diagnostic ignored \"-Wextra\"") \
	_Pragma("GCC diagnostic ignored \"-Wpedantic\"")
#	define HYPERENGINE_RESTORE_WARNINGS() _Pragma("GCC diagnostic pop")
#elif HYPERENGINE_COMPILER_MSVC
#	define HYPERENGINE_DISABLE_WARNINGS() __pragma(warning(push, 0))
#	define HYPERENGINE_RESTORE_WARNINGS() __pragma(warning(pop))
#endif
// clang-format on
