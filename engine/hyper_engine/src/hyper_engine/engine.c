/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/engine.h"

#include "hyper_common/assertion.h"
#include "hyper_common/logger.h"
#include "hyper_common/memory.h"
#include "hyper_common/vector.h"

enum hyper_result hyper_engine_create(struct hyper_engine *engine)
{
	hyper_assert$(engine != NULL);

	if (
		hyper_module_loader_create(&engine->module_loader) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$("Failed to create module loader\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	static const char *modules[] = {
		"hyper_platform",
	};

	for (size_t i = 0; i < sizeof(modules) / sizeof(modules[0]); ++i)
	{
		if (
			hyper_module_loader_load(&engine->module_loader, modules[i]) !=
			HYPER_RESULT_SUCCESS)
		{
			hyper_logger_error$("Failed to load module '%s'\n", modules[i]);
			return HYPER_RESULT_INITIALIZATION_FAILED;
		}
	}

	const struct hyper_window_create_info window_create_info = {
		.title = "HyperEngine",
		.x = 0,
		.y = 0,
		.width = 1280,
		.height = 720,
	};

	if (
		hyper_window_create(&engine->window, &window_create_info) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$("Failed to create window\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	// TODO: Hook running flag with closed event
	// engine->running = true;

	return HYPER_RESULT_SUCCESS;
}

void hyper_engine_destroy(struct hyper_engine *engine)
{
	hyper_assert$(engine != NULL);

	hyper_window_destroy(&engine->window);
	hyper_module_loader_destroy(&engine->module_loader);

	hyper_logger_debug$("Heap Summary:\n");
	hyper_logger_debug$(
		"\t%u allocations, %u frees\n",
		hyper_get_total_allocs(),
		hyper_get_total_frees());
	hyper_logger_debug$(
		"\t%u bytes allocated, %u bytes leaked\n",
		hyper_get_total_bytes(),
		hyper_get_current_bytes());
}

void hyper_engine_run(struct hyper_engine *engine)
{
	hyper_assert$(engine != NULL);

	while (engine->running)
	{
		hyper_window_update(&engine->window);
	}
}
