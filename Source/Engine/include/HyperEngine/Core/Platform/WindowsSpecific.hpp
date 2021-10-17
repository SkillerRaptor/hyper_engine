/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if !defined(_WIN32) && !defined(_WIN64)
#	error This file should only be included on Windows platforms.
#endif

#if !defined(_WIN64)
#	error Unsupported Windows 64 CPU (the only supported is x86-64).
#endif

#define HYPERENGINE_PLATFORM_WINDOWS 1

#define HYPERENGINE_DEBUG_BREAK() __debugbreak()
