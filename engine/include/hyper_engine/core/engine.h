/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_engine/platform/window.h"
#include "hyper_engine/utilities/result.h"

#include <stdbool.h>

struct hyper_engine
{
	bool running;
	struct hyper_window main_window;
};

enum hyper_result hyper_engine_create(struct hyper_engine *engine);
void hyper_engine_destroy(struct hyper_engine *engine);

void hyper_engine_run(struct hyper_engine *engine);
