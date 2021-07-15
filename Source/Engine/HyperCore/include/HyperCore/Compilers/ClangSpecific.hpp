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

#define HYPERENGINE_COMPILER_PUSH_WARNING clang diagnostic push
#define HYPERENGINE_COMPILER_POP_WARNING clang diagnostic pop
#define HYPERENGINE_COMPILER_PUSH_ANONYMOUS_STRUCT clang diagnostic ignored "-Wgnu-anonymous-struct"

#define HYPERENGINE_ALIGN(bytes) __attribute__((aligned(bytes)))
