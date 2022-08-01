/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_common/assertion.h"

#include "hyper_common/logger.h"
#include "hyper_common/platform.h"

#include <stdlib.h>
#include <string.h>

void HYPER_ASSERTion_failed(
	const char *expression,
	struct hyper_source_location source_location)
{
	const char *file_name = source_location.file_name;
#if HYPER_PLATFORM_WINDOWS
	file_name =
		(strrchr(file_name, '\\') ? strrchr(file_name, '\\') + 1 : file_name);
#elif HYPER_PLATFORM_LINUX
	file_name =
		(strrchr(file_name, '/') ? strrchr(file_name, '/') + 1 : file_name);
#endif

	hyper_logger_error(
		"Assertion '%s' failed in %s at %s:%u",
		expression,
		source_location.function_name,
		file_name,
		source_location.line);

	abort();
}
