/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/graphics_context.h"

#include "hyper_common/assertion.h"
#include "hyper_common/logger.h"
#include "hyper_common/memory.h"
#include "hyper_rendering/vulkan/vulkan_context.h"

enum hyper_result hyper_graphics_context_create(
	struct hyper_graphics_context *graphics_context,
	struct hyper_window *window)
{
	hyper_assert$(graphics_context != NULL);
	hyper_assert$(window != NULL);

	struct hyper_vulkan_context *vulkan_context =
		hyper_allocate(sizeof(struct hyper_vulkan_context));

	graphics_context->internal_context = vulkan_context;

	hyper_vulkan_context_create(vulkan_context, window);

	return HYPER_RESULT_SUCCESS;
}

void hyper_graphics_context_destroy(
	struct hyper_graphics_context *graphics_context)
{
	hyper_assert$(graphics_context != NULL);

	struct hyper_vulkan_context *vulkan_context =
		graphics_context->internal_context;

	hyper_vulkan_context_destroy(vulkan_context);

	hyper_deallocate(graphics_context->internal_context);
}

void hyper_graphics_context_render(
	struct hyper_graphics_context *graphics_context)
{
	hyper_assert$(graphics_context != NULL);

	struct hyper_vulkan_context *vulkan_context =
		graphics_context->internal_context;

	hyper_vulkan_context_render(vulkan_context);
}
