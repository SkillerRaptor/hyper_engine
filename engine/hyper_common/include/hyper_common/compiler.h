/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if defined(__clang__)
#	define HYPER_COMPILER_CLANG 1
#elif defined(__GNUC__)
#	define HYPER_COMPILER_GCC 1
#elif defined(_MSC_VER)
#	define HYPER_COMPILER_MSVC 1
#else
#	error Unsupported compiler was used.
#endif
