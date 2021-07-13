/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef HYPERENGINE_SHARED_EXPORT
#	define HYPERENGINE_API __attribute__((visibility("default")))
#else
#	define HYPERENGINE_API __attribute__((visibility("hidden")))
#endif
