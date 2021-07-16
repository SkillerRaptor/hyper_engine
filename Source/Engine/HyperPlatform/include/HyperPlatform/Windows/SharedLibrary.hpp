/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/PlatformDetection.hpp>

#ifndef HYPERENGINE_PLATFORM_WINDOWS
#	error This file should only be included on windows platforms
#endif

#ifdef HYPERENGINE_SHARED_EXPORT
#	define HYPERENGINE_API __declspec(dllexport)
#else
#	define HYPERENGINE_API __declspec(dllimport)
#endif
