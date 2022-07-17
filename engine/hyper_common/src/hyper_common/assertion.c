/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_common/assertion.h"

#include "hyper_common/file.h"
#include "hyper_common/logger.h"

void hyper_assertion_failed(
	const char *expression,
	struct hyper_source_location source_location)
{
	hyper_logger_error$(
		"Assertion '%s' failed in %s at %s:%u",
		expression,
		source_location.function_name,
		hyper_strip_file_name$(source_location.file_name),
		source_location.line);

	abort();
}
