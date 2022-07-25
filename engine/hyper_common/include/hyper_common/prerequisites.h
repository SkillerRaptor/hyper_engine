/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/debug.h"
#include "hyper_common/platform.h"

#define hyper_unused_variable$(name) ((void) name)

#if HYPER_DEBUG
#	define hyper_unused_debug_variable$(name) ((void) 0)
#else
#	define hyper_unused_debug_variable$(name) ((void) name)
#endif

#if HYPER_PLATFORM_WINDOWS
#	define hyper_unused_function$() __pragma(warning(suppress : 4505))
#elif HYPER_PLATFORM_LINUX
#	define hyper_unused_function$() __attribute__((unused))
#else
#	define hyper_unused_function$()
#endif
