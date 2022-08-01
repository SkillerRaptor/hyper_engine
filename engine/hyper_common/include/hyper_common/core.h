/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/platform.h"

#define HYPER_ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

#define HYPER_CONCATENATE_HELPER(a, b) a##b
#define HYPER_CONCATENATE(a, b) HYPER_CONCATENATE_HELPER(a, b)
#define HYPER_STRINGIFY(x) #x

#define HYPER_RESTRICT __restrict

#if HYPER_PLATFORM_WINDOWS
#	define HYPER_NORETURN __declspec(noreturn)
#elif HYPER_PLATFORM_LINUX
#	define HYPER_NORETURN __attribute__((noreturn))
#endif

#define HYPER_UNUSED_VARIABLE(name) ((void) name)

#if HYPER_PLATFORM_WINDOWS
#	define HYPER_UNUSED_FUNCTION __pragma(warning(suppress : 4505))
#elif HYPER_PLATFORM_LINUX
#	define HYPER_UNUSED_FUNCTION __attribute__((unused))
#endif
