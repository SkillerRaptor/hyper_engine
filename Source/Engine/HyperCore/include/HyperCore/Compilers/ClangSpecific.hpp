/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if !defined(__clang__)
#	error This file should only be included on Clang compilers
#endif

#define HYPERENGINE_COMPILER_CLANG 1

#define HYPERENGINE_DISABLE_WARNINGS() _Pragma("clang diagnostic push")
#define HYPERENGINE_RESTORE_WARNINGS() _Pragma("clang diagnostic pop")

#define HYPERENGINE_DISABLE_WARNING_ANONYMOUS_STRUCT()          \
	HYPERENGINE_DISABLE_WARNINGS()                              \
	_Pragma("clang diagnostic ignored \"-Wnested-anon-types\"") \
	_Pragma("clang diagnostic ignored \"-Wgnu-anonymous-struct\"")

#define HYPERENGINE_RESTORE_WARNING_ANONYMOUS_STRUCT() \
	HYPERENGINE_RESTORE_WARNINGS()

#define HYPERENGINE_SHARED_EXPORT __attribute__((visibility("default")))
#define HYPERENGINE_SHARED_IMPORT __attribute__((visibility("hidden")))

#define HYPERENGINE_ALIGN(bytes) __attribute__((aligned(bytes)))
#define HYPERENGINE_FORCE_INLINE __attribute__((noinline))
#define HYPERENGINE_NO_INLINE __attribute__((always_inline))

#define HYPERENGINE_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
