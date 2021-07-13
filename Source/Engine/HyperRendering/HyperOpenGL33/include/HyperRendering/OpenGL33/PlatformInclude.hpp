/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#	include <HyperRendering/OpenGL33/Windows/PlatformInclude.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
#	include <HyperRendering/OpenGL33/Linux/PlatformInclude.hpp>
#endif
