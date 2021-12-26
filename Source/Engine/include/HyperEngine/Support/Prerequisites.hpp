/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if !defined(_DEBUG) && !defined(ADEBUG) && !defined(NDEBUG)
#	define NDEBUG
#endif

#ifndef NDEBUG
#	define HYPERENGINE_DEBUG 1
#else
#	define HYPERENGINE_DEBUG 0
#endif

#define HYPERENGINE_NON_COPYABLE(class_name)    \
	class_name(const class_name &other) = delete; \
	class_name &operator=(const class_name &other) = delete

#define HYPERENGINE_NON_MOVABLE(class_name)         \
	class_name(class_name &&other) noexcept = delete; \
	class_name &operator=(class_name &&other) noexcept = delete
