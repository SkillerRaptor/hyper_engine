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
#include "hyper_game/key_events.h"
#include "hyper_game/mouse_events.h"
#include "hyper_game/window_events.h"
#include "hyper_platform/window.h"

#define HYPER_EVENT_BUS_CALLBACKS \
	X(key_press);                   \
	X(key_release);                 \
	X(key_type);                    \
	X(mouse_button_press);          \
	X(mouse_button_release);        \
	X(mouse_move);                  \
	X(mouse_scroll);                \
	X(window_close);                \
	X(window_move);                 \
	X(window_resize);               \
	X(window_framebuffer_resize);

#define X(name)                                           \
	typedef void (*hyper_event_##name##_callback_function)( \
		struct hyper_##name##_event name##_event, void *user_data)
HYPER_EVENT_BUS_CALLBACKS
#undef X

struct hyper_event_bus
{
#define X(name) struct hyper_vector name##_callbacks
	HYPER_EVENT_BUS_CALLBACKS
#undef X
};

HYPER_API enum hyper_result hyper_event_bus_create(
	struct hyper_event_bus *event_bus,
	struct hyper_window *window);
HYPER_API void hyper_event_bus_destroy(struct hyper_event_bus *event_bus);

#define X(name)                                             \
	HYPER_API void hyper_register_##name##_callback(          \
		struct hyper_event_bus *event_bus,                      \
		hyper_event_##name##_callback_function name##_callback, \
		void *user_data)
HYPER_EVENT_BUS_CALLBACKS
#undef X
