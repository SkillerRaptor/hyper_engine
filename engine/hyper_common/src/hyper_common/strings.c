/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_common/strings.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

char *hyper_strings_concatenate(size_t count, ...)
{
	va_list args;

	size_t length = 1;
	va_start(args, count);
	for (size_t i = 0; i < count; ++i)
	{
		length += strlen(va_arg(args, char *));
	}
	va_end(args);

	char *string = calloc(1, sizeof(char) * length);
	if (string == NULL)
	{
		return NULL;
	}

	size_t last_position = 0;
	va_start(args, count);
	for (size_t i = 0; i < count; ++i)
	{
		char *string_destination = string + last_position;
		const char *part_string = va_arg(args, char *);
		strcpy(string_destination, part_string);

		const size_t part_string_length = strlen(part_string);
		last_position += part_string_length;
	}
	va_end(args);

	return string;
}
