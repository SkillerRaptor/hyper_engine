/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/library.h"
#include "hyper_common/result.h"
#include "hyper_platform/window.h"

struct hyper_graphics_context
{
	void *internal_context;
};

HYPER_API enum hyper_result hyper_graphics_context_create(
	struct hyper_graphics_context *graphics_context,
	struct hyper_window *window);
HYPER_API void hyper_graphics_context_destroy(
	struct hyper_graphics_context *graphics_context);
