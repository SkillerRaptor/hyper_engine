/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if !defined(__linux__) && !defined(__linux)
#	error This file should only be included on Linux platforms.
#endif

#if !HYPERENGINE_ARCHITECTURE_X86_64
#	error Unsupported Linux CPU (the only supported are x86-64).
#endif

#include <csignal>

#define HYPERENGINE_PLATFORM_LINUX 1

#define HYPERENGINE_DEBUG_BREAK() std::raise(SIGTRAP)
