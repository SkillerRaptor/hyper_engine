/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#if !defined(_DEBUG) && !defined(ADEBUG) && !defined(NDEBUG)
#	define NDEBUG
#endif

#ifdef NDEBUG
#	define HYPERENGINE_DEBUG 0
#else
#	define HYPERENGINE_DEBUG 1
#endif

#define HYPERENGINE_UNUSED_VARIABLE(x) (void) (x)
#define HYPERENGINE_UNUSED_VARIADIC(x) (HYPERENGINE_UNUSED_VARIABLE(x), ...)

#define HYPERENGINE_UNREACHABLE() std::abort()

#define HYPERENGINE_NON_COPYABLE(class_name)    \
	class_name(const class_name &other) = delete; \
	class_name &operator=(const class_name &other) = delete

#define HYPERENGINE_NON_MOVABLE(class_name)         \
	class_name(class_name &&other) noexcept = delete; \
	class_name &operator=(class_name &&other) noexcept = delete
