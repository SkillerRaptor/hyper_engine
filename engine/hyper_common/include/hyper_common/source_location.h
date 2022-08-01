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

#define HYPER_LINE_CURRENT() __LINE__
#define HYPER_FILE_CURRENT() __FILE__

#if defined(__FUNCTION__)
#	define HYPER_FUNCTION_CURRENT() __FUNCTION__
#elif defined(__func__)
#	define HYPER_FUNCTION_CURRENT() __func__
#elif defined(__PRETTY_FUNCTION__)
#	define HYPER_FUNCTION_CURRENT() __PRETTY_FUNCTION__
#else
#	define HYPER_FUNCTION_CURRENT() "unknown"
#endif

#define HYPER_SOURCE_LOCATION_CURRENT()        \
	((struct hyper_source_location){             \
		.line = HYPER_LINE_CURRENT(),              \
		.file_name = HYPER_FILE_CURRENT(),         \
		.function_name = HYPER_FUNCTION_CURRENT(), \
	})
