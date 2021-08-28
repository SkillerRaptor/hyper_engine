/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if defined(__x86_64__) || defined(_M_X64)
#	define HYPERENGINE_PLATFORM_X86_64 1
#elif defined(__i386) || defined(_M_IX86) || defined(__arm__)
#	error 32-bit platforms are not supported.
#else
#	define HYPERENGINE_PLATFORM_UNKNOWN_CPU 1
#endif

#if defined(_WIN32)
#	define HYPERENGINE_PLATFORM_WINDOWS 1
#	if !defined(_WIN64)
#		error Unsupported Windows 64 CPU (the only supported is x86-64).
#	endif
#elif defined(__linux__) || defined(__linux)
#	define HYPERENGINE_PLATFORM_LINUX 1
#	if !HYPERENGINE_PLATFORM_X86_64
#		error Unsupported Linux CPU (the only supported are x86-64).
#	endif
#else
#	error Unknown target platform.
#endif
