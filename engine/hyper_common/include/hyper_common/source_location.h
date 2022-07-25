/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

struct hyper_source_location
{
	uint32_t line;
	const char *file_name;
	const char *function_name;
};

#if defined(__FUNCTION__)
#	define hyper_current_file$() __FUNCTION__
#else
#	define hyper_current_file$() __func__
#endif

#define hyper_source_location_current$()    \
	((struct hyper_source_location){          \
		.line = __LINE__,                       \
		.file_name = __FILE__,                  \
		.function_name = hyper_current_file$(), \
	})
