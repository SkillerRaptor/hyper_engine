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

#if HYPERENGINE_COMPILER_CLANG || HYPERENGINE_COMPILER_GCC
#	define HYPERENGINE_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#elif HYPERENGINE_COMPILER_MSVC
#	define HYPERENGINE_FUNCTION_SIGNATURE __FUNCSIG__
#endif
