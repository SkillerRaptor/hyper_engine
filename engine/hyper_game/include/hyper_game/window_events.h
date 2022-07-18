/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

struct hyper_window_close_event
{
	uint8_t unused;
};

struct hyper_window_move_event
{
	uint32_t position_x;
	uint32_t position_y;
};

struct hyper_window_resize_event
{
	uint32_t width;
	uint32_t height;
};
