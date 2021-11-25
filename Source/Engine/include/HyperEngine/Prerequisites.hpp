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
