/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

#if defined(__x86_64__) || defined(_M_X64)
#	define HYPERENGINE_ARCHITECTURE_X86_64 1
#elif defined(__i386) || defined(_M_IX86)
#	error 32-bit architectures are not supported.
#else
#	error Unsupported architecture was used.
#endif

#if defined(__linux__) || defined(__linux)
#	include "HyperEngine/Core/Platform/LinuxSpecific.hpp"
#elif defined(_WIN32) || defined(_WIN64)
#	include "HyperEngine/Core/Platform/WindowsSpecific.hpp"
#else
#	error Unsupported platform was used.
#endif

namespace HyperEngine
{
	enum class EPlatform : uint8_t
	{
		None = 0,
		Linux,
		Windows,

#if HYPERENGINE_PLATFORM_LINUX
		Current = Linux
#elif HYPERENGINE_PLATFORM_WINDOWS
		Current = Windows
#else
		Current = None
#endif
	};
} // namespace HyperEngine