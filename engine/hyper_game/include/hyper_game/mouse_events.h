/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_game/mouse_codes.h"

struct hyper_mouse_button_press_event
{
	enum hyper_mouse_code mouse_code;
};

struct hyper_mouse_button_release_event
{
	enum hyper_mouse_code mouse_code;
};

struct hyper_mouse_move_event
{
	float position_x;
	float position_y;
};

struct hyper_mouse_scroll_event
{
	float offset_x;
	float offset_y;
};
