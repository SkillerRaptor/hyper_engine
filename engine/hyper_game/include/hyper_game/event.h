/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_game/key_events.h"
#include "hyper_game/mouse_events.h"
#include "hyper_game/window_events.h"

enum hyper_event_type
{
	HYPER_EVENT_TYPE_KEY_PRESS,
	HYPER_EVENT_TYPE_KEY_RELEASE,
	HYPER_EVENT_TYPE_KEY_TYPE,

	HYPER_EVENT_TYPE_MOUSE_BUTTON_PRESS,
	HYPER_EVENT_TYPE_MOUSE_BUTTON_RELEASE,
	HYPER_EVENT_TYPE_MOUSE_MOVE,
	HYPER_EVENT_TYPE_MOUSE_SCROLL,

	HYPER_EVENT_TYPE_WINDOW_CLOSE,
	HYPER_EVENT_TYPE_WINDOW_MOVE,
	HYPER_EVENT_TYPE_WINDOW_RESIZE,
	HYPER_EVENT_TYPE_WINDOW_FRAMEBUFFER_RESIZE,
};

struct hyper_event
{
	enum hyper_event_type type;

	union
	{
		struct hyper_key_press_event key_press_event;
		struct hyper_key_release_event key_release_event;
		struct hyper_key_type_event key_type_event;

		struct hyper_mouse_button_press_event mouse_button_press_event;
		struct hyper_mouse_button_release_event mouse_button_release_event;
		struct hyper_mouse_move_event mouse_move_event;
		struct hyper_mouse_scroll_event mouse_scroll_event;

		struct hyper_window_close_event window_close_event;
		struct hyper_window_move_event window_move_event;
		struct hyper_window_resize_event window_resize_event;
		struct hyper_window_framebuffer_resize_event
			window_framebuffer_resize_event;
	};
};
