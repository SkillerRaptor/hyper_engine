/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if !defined(__clang__)
#	error This file should only be included on clang compilers
#endif

#define HYPERENGINE_COMPILER_CLANG 1

#define HYPERENGINE_COMPILER_PUSH_WARNING _Pragma("clang diagnostic push")
#define HYPERENGINE_COMPILER_POP_WARNING _Pragma("clang diagnostic pop")
#define HYPERENGINE_COMPILER_PUSH_ANONYMOUS_STRUCT \
	_Pragma("clang diagnostic ignored \"-Wgnu-anonymous-struct\"") \
	_Pragma("clang diagnostic ignored \"-Wnested-anon-types\"")

#define HYPERENGINE_ALIGN(bytes) __attribute__((aligned(bytes)))
