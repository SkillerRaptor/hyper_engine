/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/debug.h"
#include "hyper_common/source_location.h"

void HYPER_ASSERTion_failed(
	const char *expression,
	struct hyper_source_location source_location);

#if HYPER_DEBUG
#	define HYPER_ASSERT(expression)                                            \
		do                                                                        \
		{                                                                         \
			if (!(expression))                                                      \
			{                                                                       \
				HYPER_ASSERTion_failed(#expression, HYPER_SOURCE_LOCATION_CURRENT()); \
			}                                                                       \
		} while (0)
#else
#	define HYPER_ASSERT(expression) ((void) 0)
#endif
