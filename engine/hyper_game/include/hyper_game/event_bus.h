/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/library.h"
#include "hyper_common/queue.h"
#include "hyper_common/result.h"
#include "hyper_common/vector.h"
#include "hyper_game/event.h"
#include "hyper_game/key_events.h"
#include "hyper_game/mouse_events.h"
#include "hyper_game/window_events.h"
#include "hyper_platform/window.h"

#define HYPER_EVENT_BUS_CALLBACKS                                 \
	X(key_press, HYPER_EVENT_TYPE_KEY_PRESS);                       \
	X(key_release, HYPER_EVENT_TYPE_KEY_RELEASE);                   \
	X(key_type, HYPER_EVENT_TYPE_KEY_TYPE);                         \
	X(mouse_button_press, HYPER_EVENT_TYPE_MOUSE_BUTTON_PRESS);     \
	X(mouse_button_release, HYPER_EVENT_TYPE_MOUSE_BUTTON_RELEASE); \
	X(mouse_move, HYPER_EVENT_TYPE_MOUSE_MOVE);                     \
	X(mouse_scroll, HYPER_EVENT_TYPE_MOUSE_SCROLL);                 \
	X(window_close, HYPER_EVENT_TYPE_WINDOW_CLOSE);                 \
	X(window_move, HYPER_EVENT_TYPE_WINDOW_MOVE);                   \
	X(window_resize, HYPER_EVENT_TYPE_WINDOW_RESIZE);               \
	X(window_framebuffer_resize, HYPER_EVENT_TYPE_WINDOW_FRAMEBUFFER_RESIZE);

#define X(name, type)                                     \
	typedef void (*hyper_event_##name##_callback_function)( \
		struct hyper_##name##_event name##_event, void *user_data)
HYPER_EVENT_BUS_CALLBACKS
#undef X

struct hyper_event_bus
{
	struct hyper_queue event_list;

#define X(name, type) struct hyper_vector name##_callbacks
	HYPER_EVENT_BUS_CALLBACKS
#undef X
};

HYPER_API enum hyper_result hyper_event_bus_create(
	struct hyper_event_bus *event_bus,
	struct hyper_window *window);
HYPER_API void hyper_event_bus_destroy(struct hyper_event_bus *event_bus);

HYPER_API void hyper_event_bus_process(struct hyper_event_bus *event_bus);

#define X(name, type)                                             \
	HYPER_API void hyper_register_##name##_callback(          \
		struct hyper_event_bus *event_bus,                      \
		hyper_event_##name##_callback_function name##_callback, \
		void *user_data)
HYPER_EVENT_BUS_CALLBACKS
#undef X
