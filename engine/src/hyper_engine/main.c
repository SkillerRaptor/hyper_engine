/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/utilities/assertion.h"
#include "hyper_engine/utilities/logger.h"

#include <stdio.h>

int main()
{
	logger_info("Hello, World!");
	logger_warn("Hello, World!");
	logger_error("Hello, World!");
	logger_debug("Hello, World!");
	assert$(69 == 420);

	return 0;
}
