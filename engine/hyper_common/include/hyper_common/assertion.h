/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/debug.h"
#include "hyper_common/source_location.h"

#include <stdlib.h>

#if HYPER_DEBUG
#	define hyper_assert$(expression)                                            \
		do                                                                         \
		{                                                                          \
			if (!(expression))                                                       \
			{                                                                        \
				hyper_assertion_failed(#expression, hyper_source_location_current$()); \
			}                                                                        \
		} while (0)
#	define hyper_asssert_not_reached$() assert$(false)
#else
#	define hyper_assert$(expression) ((void) 0)
#	define hyper_asssert_not_reached$() abort()
#endif

void hyper_assertion_failed(
	const char *expression,
	struct hyper_source_location source_location);
