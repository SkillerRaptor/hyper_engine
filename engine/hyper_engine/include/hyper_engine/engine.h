/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/debug.h"
#include "hyper_common/memory.h"
#include "hyper_common/result.h"
#include "hyper_engine/module_loader.h"
#include "hyper_game/event_bus.h"
#include "hyper_platform/window.h"
#include "hyper_rendering/graphics_context.h"

#include <stdbool.h>

struct hyper_engine
{
	bool running;

#if HYPER_DEBUG
	struct hyper_memory_info memory_info;
#endif

	struct hyper_module_loader module_loader;
	struct hyper_window window;
	struct hyper_event_bus event_bus;
	struct hyper_graphics_context graphics_context;
};

enum hyper_result hyper_engine_create(struct hyper_engine *engine);
void hyper_engine_destroy(struct hyper_engine *engine);

void hyper_engine_run(struct hyper_engine *engine);
