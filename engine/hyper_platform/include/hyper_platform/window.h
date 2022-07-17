/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/library.h"
#include "hyper_common/result.h"

#include <stdint.h>

struct hyper_window
{
	void *native_window;
};

struct hyper_window_create_info
{
	const char *title;
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
};

HYPER_API enum hyper_result hyper_window_create(
	struct hyper_window *window,
	const struct hyper_window_create_info *window_create_info);
HYPER_API void hyper_window_destroy(struct hyper_window *window);

HYPER_API void hyper_window_update(struct hyper_window *window);
