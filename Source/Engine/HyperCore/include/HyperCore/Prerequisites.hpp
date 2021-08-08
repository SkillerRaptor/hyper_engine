/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define HYPERENGINE_STRINGIFY_HELPER(x) #x
#define HYPERENGINE_STRINGIFY(x) HYPERENGINE_STRINGIFY_HELPER(x)
#define HYPERENGINE_EXPAND_MACRO(x) x

#define HYPERENGINE_NON_COPYABLE(class_name) \
	class_name(const class_name&) = delete;  \
	class_name& operator=(const class_name&) = delete

#define HYPERENGINE_NON_MOVEABLE(class_name)    \
	class_name(class_name&&) = delete; \
	class_name& operator=(class_name&&) = delete

#ifndef NDEBUG
#	define HYPERENGINE_DEBUG 1
#else
#	define HYPERENGINE_RELEASE 1
#endif
