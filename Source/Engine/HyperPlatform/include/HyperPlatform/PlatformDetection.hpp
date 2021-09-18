/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if defined(__x86_64__) || defined(_M_X64)
#	define HYPERENGINE_ARCHITECTURE_X86_64 1
#elif defined(__i386) || defined(_M_IX86) || defined(__arm__)
#	error 32-bit architectures are not supported.
#else
#	error Unsupported architecture was used.
#endif

#if defined(_WIN32)
#	define HYPERENGINE_PLATFORM_WINDOWS 1
#	if !defined(_WIN64)
#		error Unsupported Windows 64 CPU (the only supported is x86-64).
#	endif
#elif defined(__linux__) || defined(__linux)
#	define HYPERENGINE_PLATFORM_LINUX 1
#	if !HYPERENGINE_ARCHITECTURE_X86_64
#		error Unsupported Linux CPU (the only supported are x86-64).
#	endif
#else
#	error Unsupported target platform was used.
#endif
