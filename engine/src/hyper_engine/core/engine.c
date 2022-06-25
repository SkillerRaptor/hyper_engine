/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/core/engine.h"

#include "hyper_engine/utilities/assertion.h"
#include "hyper_engine/utilities/logger.h"

enum hyper_result hyper_engine_create(struct hyper_engine *engine)
{
	assert$(engine != NULL);

	const struct hyper_window_create_info window_create_info = {
		.title = "HyperEngine",
		.width = 1280,
		.height = 720,
	};

	const enum hyper_result window_result =
		hyper_window_create(&engine->main_window, &window_create_info);
	if (window_result != HYPER_RESULT_SUCCESS)
	{
		logger_error("engine: failed to create window");
		return window_result;
	}

	engine->running = true;

	return HYPER_RESULT_SUCCESS;
}

void hyper_engine_destroy(struct hyper_engine *engine)
{
	assert$(engine != NULL);

	hyper_window_destroy(&engine->main_window);
}

void hyper_engine_run(struct hyper_engine *engine)
{
	assert$(engine != NULL);

	// TODO: Replace window close bool with engine running bool by using events
	while (!engine->main_window.closed)
	{
		hyper_window_update(&engine->main_window);
	}
}
