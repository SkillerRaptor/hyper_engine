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
#	define HYPERENGINE_COMPILER_VC 1
#endif
