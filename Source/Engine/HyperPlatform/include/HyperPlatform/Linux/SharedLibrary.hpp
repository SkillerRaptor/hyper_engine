/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/PlatformDetection.hpp>

#ifndef HYPERENGINE_PLATFORM_LINUX
#	error This file should only be included on linux platforms
#endif

#ifdef HYPERENGINE_SHARED_EXPORT
#	define HYPERENGINE_API __attribute__((visibility("default")))
#else
#	define HYPERENGINE_API __attribute__((visibility("hidden")))
#endif
