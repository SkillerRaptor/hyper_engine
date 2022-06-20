/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/utilities/assertion.h"

#include "hyper_engine/utilities/logger.h"

#include <stdlib.h>

void assertion_failed(
	const char *assertion,
	struct source_location source_location)
{
	logger_error(
		"Assertion '%s' failed in %s at %s:%u",
		assertion,
		source_location.function_name,
		source_location.file_name,
		source_location.line);

	abort();
}
