/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/graphics_context.h"

#include "hyper_common/assertion.h"
#include "hyper_common/logger.h"
#include "hyper_rendering/vulkan/vulkan_context.h"

#include <stdlib.h>

enum hyper_result hyper_graphics_context_create(
	struct hyper_graphics_context *graphics_context,
	struct hyper_window *window)
{
	HYPER_ASSERT(graphics_context != NULL);
	HYPER_ASSERT(window != NULL);

	graphics_context->vulkan_context =
		calloc(1, sizeof(struct hyper_vulkan_context));
	if (graphics_context->vulkan_context == NULL)
	{
		hyper_logger_error("Failed to allocate vulkan context\n");
		return HYPER_RESULT_OUT_OF_MEMORY;
	}

	if (
		hyper_vulkan_context_create(graphics_context->vulkan_context, window) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error("Failed to create vulkan context\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	hyper_logger_info("Successfully created graphics context\n");

	return HYPER_RESULT_SUCCESS;
}

void hyper_graphics_context_destroy(
	struct hyper_graphics_context *graphics_context)
{
	if (graphics_context == NULL)
	{
		return;
	}

	hyper_vulkan_context_destroy(graphics_context->vulkan_context);
	free(graphics_context->vulkan_context);

	hyper_logger_info("Successfully destroyed graphics context\n");
}

void hyper_graphics_context_begin_frame(
	struct hyper_graphics_context *graphics_context)
{
	HYPER_ASSERT(graphics_context != NULL);
	HYPER_UNUSED_VARIABLE(graphics_context);

	//hyper_vulkan_context_render(graphics_context->vulkan_context);
}

void hyper_graphics_context_end_frame(
	struct hyper_graphics_context *graphics_context)
{
	HYPER_ASSERT(graphics_context != NULL);

	hyper_vulkan_context_render(graphics_context->vulkan_context);
}
