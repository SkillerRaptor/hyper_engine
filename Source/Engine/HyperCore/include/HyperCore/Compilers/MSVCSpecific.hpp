/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if !defined(_MSC_VER)
#	error This file should only be included on MSVC compiler
#endif

#define HYPERENGINE_COMPILER_MSVC 1

#define HYPERENGINE_DISABLE_WARNINGS() __pragma(warning(push))
#define HYPERENGINE_RESTORE_WARNINGS() __pragma(warning(pop))

#define HYPERENGINE_DISABLE_WARNING_ANONYMOUS_STRUCT() \
	HYPERENGINE_DISABLE_WARNINGS()                     \
	__pragma(warning(disable : 4201))

#define HYPERENGINE_RESTORE_WARNING_ANONYMOUS_STRUCT() \
	HYPERENGINE_RESTORE_WARNINGS()

#define HYPERENGINE_SHARED_EXPORT __declspec(dllexport)
#define HYPERENGINE_SHARED_IMPORT __declspec(dllimport)

#define HYPERENGINE_ALIGN(bytes) __declspec(align(bytes))
#define HYPERENGINE_FORCE_INLINE __declspec(noinline)
#define HYPERENGINE_NO_INLINE __forceinline

#define HYPERENGINE_FUNCTION_SIGNATURE __FUNCSIG__
