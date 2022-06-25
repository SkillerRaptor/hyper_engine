/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/core/engine.h"

int main()
{
	struct hyper_engine engine = { 0 };
	hyper_engine_create(&engine);
	hyper_engine_run(&engine);
	hyper_engine_destroy(&engine);
	return 0;
}
