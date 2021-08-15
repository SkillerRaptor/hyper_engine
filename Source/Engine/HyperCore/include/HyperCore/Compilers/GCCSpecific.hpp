/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if !defined(__GNUC__) && !defined(__GNUG__)
#	error This file should only be included on gcc compilers
#endif

#define HYPERENGINE_COMPILER_GCC 1

#define HYPERENGINE_COMPILER_PUSH_WARNING _Pragma("GCC diagnostic push")
#define HYPERENGINE_COMPILER_POP_WARNING _Pragma("GCC diagnostic pop")
#define HYPERENGINE_COMPILER_PUSH_ANONYMOUS_STRUCT _Pragma("GCC diagnostic ignored \"-Wpedantic\"")

#define HYPERENGINE_FUNCTION_SIGNATURE __PRETTY_FUNCTION__

#define HYPERENGINE_ALIGN(bytes) __attribute__((aligned(bytes)))
