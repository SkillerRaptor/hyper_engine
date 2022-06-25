/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_engine/utilities/source_location.h"

#ifdef HYPERENGINE_DEBUG
#	define assert$(assertion)                                      \
		do                                                            \
		{                                                             \
			if (!(assertion))                                           \
			{                                                           \
				assertion_failed(#assertion, source_location_current$()); \
			}                                                           \
		} while (0)
#	define asssert_not_reached$() assert$(false)
#else
#	define assert$(assertion) ((void) 0)
#	define asssert_not_reached$() abort()
#endif

void assertion_failed(
	const char *assertion,
	struct source_location source_location);
