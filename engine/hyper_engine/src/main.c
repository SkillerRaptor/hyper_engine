/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_common/core.h"
#include "hyper_engine/engine.h"

#include <stdlib.h>

int main(int argc, char **argv)
{
	HYPER_UNUSED_VARIABLE(argc);
	HYPER_UNUSED_VARIABLE(argv);

	struct hyper_engine engine = { 0 };
	hyper_engine_create(&engine);
	hyper_engine_run(&engine);
	hyper_engine_destroy(&engine);

	return EXIT_SUCCESS;
}
