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

#define hyper_source_location_current$() \
	((struct hyper_source_location){       \
		.line = __LINE__,                    \
		.file_name = __FILE__,               \
		.function_name = __FUNCTION__,       \
	})
