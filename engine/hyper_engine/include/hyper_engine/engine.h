/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/result.h"
#include "hyper_engine/module_loader.h"
#include "hyper_platform/window.h"

#include <stdbool.h>

struct hyper_engine
{
	bool running;
	struct hyper_module_loader module_loader;
	struct hyper_window window;
};

enum hyper_result hyper_engine_create(struct hyper_engine *engine);
void hyper_engine_destroy(struct hyper_engine *engine);

void hyper_engine_run(struct hyper_engine *engine);
