/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

struct source_location
{
	uint32_t line;
	const char *file_name;
	const char *function_name;
};

#define source_location_current$() \
	((struct source_location){       \
		.line = __LINE__,              \
		.file_name = __FILE__,         \
		.function_name = __FUNCTION__, \
	})
