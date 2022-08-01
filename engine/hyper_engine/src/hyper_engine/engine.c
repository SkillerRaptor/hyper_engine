/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/engine.h"

#include "hyper_common/assertion.h"
#include "hyper_common/core.h"
#include "hyper_common/logger.h"
#include "hyper_common/vector.h"

static void hyper_window_close_callback(
	struct hyper_window_close_event window_close_event,
	void *user_data)
{
	HYPER_UNUSED_VARIABLE(window_close_event);

	struct hyper_engine *engine = user_data;
	engine->running = false;
}

enum hyper_result hyper_engine_create(struct hyper_engine *engine)
{
	HYPER_ASSERT(engine != NULL);

	if (
		hyper_module_loader_create(&engine->module_loader) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error("Failed to create module loader\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	static const char *modules[] = {
		"hyper_game",
		"hyper_math",
		"hyper_platform",
		"hyper_rendering",
	};

	for (size_t i = 0; i < HYPER_ARRAY_SIZE(modules); ++i)
	{
		if (
			hyper_module_loader_load(&engine->module_loader, modules[i]) !=
			HYPER_RESULT_SUCCESS)
		{
			hyper_logger_error("Failed to load module '%s'\n", modules[i]);
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
		hyper_logger_error("Failed to create window\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_event_bus_create(&engine->event_bus, &engine->window) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error("Failed to create event bus\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}
	
	hyper_event_bus_register_window_close(
		&engine->event_bus, hyper_window_close_callback, engine);

	if (
		hyper_graphics_context_create(&engine->graphics_context, &engine->window) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error("Failed to create graphics context\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	engine->running = true;

	return HYPER_RESULT_SUCCESS;
}

void hyper_engine_destroy(struct hyper_engine *engine)
{
	if (engine == NULL)
	{
		return;
	}

	hyper_graphics_context_destroy(&engine->graphics_context);
	hyper_event_bus_destroy(&engine->event_bus);
	hyper_window_destroy(&engine->window);
	hyper_module_loader_destroy(&engine->module_loader);
}

void hyper_engine_run(struct hyper_engine *engine)
{
	HYPER_ASSERT(engine != NULL);

	while (engine->running)
	{
		hyper_window_poll_events();

		hyper_graphics_context_begin_frame(&engine->graphics_context);
		hyper_graphics_context_end_frame(&engine->graphics_context);
	}
}
