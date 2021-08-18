/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifndef NDEBUG
#	define HYPERENGINE_DEBUG 1
#else
#	define HYPERENGINE_RELEASE 1
#endif

#define HYPERENGINE_STRINGIFY_HELPER(x) #x
#define HYPERENGINE_STRINGIFY(x) HYPERENGINE_STRINGIFY_HELPER(x)
#define HYPERENGINE_EXPAND_MACRO(x) x

#define HYPERENGINE_VARIABLE_NOT_USED(x) ((void) (x))
#define HYPERENGINE_VARIADIC_NOT_USED(x) (((void) (x)), ...)

#define HYPERENGINE_NON_COPYABLE(class_name) \
	class_name(const class_name&) = delete;  \
	auto operator=(const class_name&) -> class_name& = delete

#define HYPERENGINE_NON_MOVABLE(class_name)     \
	class_name(class_name&&) noexcept = delete; \
	auto operator=(class_name&&) noexcept -> class_name& = delete

#define HYPERENGINE_SINGLETON(class_name) \
	class_name() = delete;                \
	~class_name() = delete;               \
	HYPERENGINE_NON_COPYABLE(class_name); \
	HYPERENGINE_NON_MOVABLE(class_name)
