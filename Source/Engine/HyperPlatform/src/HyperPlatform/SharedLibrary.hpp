/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <HyperPlatform/Windows/SharedLibrary.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
#include <HyperPlatform/Linux/SharedLibrary.hpp>
#endif
