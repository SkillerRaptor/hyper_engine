/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#	ifdef HYPERENGINE_SHARED_EXPORT
#		define HYPERENGINE_API __declspec(dllexport)
#	else
#		define HYPERENGINE_API __declspec(dllimport)
#	endif
#endif
