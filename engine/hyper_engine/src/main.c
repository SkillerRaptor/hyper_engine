/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/engine.h"

#include "hyper_common/prerequisites.h"

#include <stdlib.h>

int main(int argc, char **argv)
{
	hyper_unused_variable$(argc);
	hyper_unused_variable$(argv);

	struct hyper_engine engine = { 0 };
	hyper_engine_create(&engine);
	hyper_engine_run(&engine);
	hyper_engine_destroy(&engine);

	return EXIT_SUCCESS;
}
