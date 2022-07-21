/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_game/event_bus.h"

#include "hyper_common/assertion.h"
#include "hyper_common/logger.h"

#define X(name, type)                                \
	struct hyper_event_##name##_data                   \
	{                                                  \
		hyper_event_##name##_callback_function callback; \
		void *user_data;                                 \
	}
HYPER_EVENT_BUS_CALLBACKS
#undef X

static void hyper_key_callback(
	struct hyper_window *window,
	enum hyper_key_action key_action,
	uint32_t key_code,
	void *user_data)
{
	struct hyper_event_bus *event_bus = user_data;

	switch (key_action)
	{
	case HYPER_KEY_ACTION_PRESS:
	case HYPER_KEY_ACTION_REPEAT:
	{
		struct hyper_key_press_event key_press_event = {
			.repeat = (key_action == HYPER_KEY_ACTION_REPEAT),
			.key_code = key_code,
		};
		struct hyper_event event_data = {
			.type = HYPER_EVENT_TYPE_KEY_PRESS,
			.key_press_event = key_press_event,
		};

		hyper_queue_push(&event_bus->event_list, &event_data);
		break;
	}
	case HYPER_KEY_ACTION_RELEASE:
	{
		struct hyper_key_release_event key_release_event = {
			.key_code = key_code,
		};
		struct hyper_event event_data = {
			.type = HYPER_EVENT_TYPE_KEY_RELEASE,
			.key_release_event = key_release_event,
		};

		hyper_queue_push(&event_bus->event_list, &event_data);
		break;
	}
	default:
		break;
	}
}

static void hyper_key_type_callback(
	struct hyper_window *window,
	uint32_t key_code,
	void *user_data)
{
	struct hyper_event_bus *event_bus = user_data;

	struct hyper_key_type_event key_type_event = {
		.key_code = key_code,
	};
	struct hyper_event event_data = {
		.type = HYPER_EVENT_TYPE_KEY_TYPE,
		.key_type_event = key_type_event,
	};

	hyper_queue_push(&event_bus->event_list, &event_data);
}

static void hyper_mouse_button_callback(
	struct hyper_window *window,
	enum hyper_mouse_action mouse_action,
	uint32_t mouse_code,
	void *user_data)
{
	struct hyper_event_bus *event_bus = user_data;

	switch (mouse_action)
	{
	case HYPER_MOUSE_ACTION_PRESS:
	{
		struct hyper_mouse_button_press_event mouse_button_press_event = {
			.mouse_code = mouse_code,
		};
		struct hyper_event event_data = {
			.type = HYPER_EVENT_TYPE_MOUSE_BUTTON_PRESS,
			.mouse_button_press_event = mouse_button_press_event,
		};

		hyper_queue_push(&event_bus->event_list, &event_data);
		break;
	}
	case HYPER_MOUSE_ACTION_RELEASE:
	{
		struct hyper_mouse_button_release_event mouse_button_release_event = {
			.mouse_code = mouse_code,
		};
		struct hyper_event event_data = {
			.type = HYPER_EVENT_TYPE_MOUSE_BUTTON_RELEASE,
			.mouse_button_release_event = mouse_button_release_event,
		};

		hyper_queue_push(&event_bus->event_list, &event_data);
		break;
	}
	default:
		break;
	}
}

static void hyper_mouse_move_callback(
	struct hyper_window *window,
	float position_x,
	float position_y,
	void *user_data)
{
	struct hyper_event_bus *event_bus = user_data;

	struct hyper_mouse_move_event mouse_move_event = {
		.position_x = position_x,
		.position_y = position_y,
	};
	struct hyper_event event_data = {
		.type = HYPER_EVENT_TYPE_MOUSE_MOVE,
		.mouse_move_event = mouse_move_event,
	};

	hyper_queue_push(&event_bus->event_list, &event_data);
}

static void hyper_mouse_scroll_callback(
	struct hyper_window *window,
	float offset_x,
	float offset_y,
	void *user_data)
{
	struct hyper_event_bus *event_bus = user_data;

	struct hyper_mouse_scroll_event mouse_scroll_event = {
		.offset_x = offset_x,
		.offset_y = offset_y,
	};
	struct hyper_event event_data = {
		.type = HYPER_EVENT_TYPE_MOUSE_SCROLL,
		.mouse_scroll_event = mouse_scroll_event,
	};

	hyper_queue_push(&event_bus->event_list, &event_data);
}

static void hyper_window_close_callback(
	struct hyper_window *window,
	void *user_data)
{
	struct hyper_event_bus *event_bus = user_data;

	struct hyper_window_close_event window_close_event = { 0 };
	struct hyper_event event_data = {
		.type = HYPER_EVENT_TYPE_WINDOW_CLOSE,
		.window_close_event = window_close_event,
	};

	hyper_queue_push(&event_bus->event_list, &event_data);
}

static void hyper_window_move_callback(
	struct hyper_window *window,
	uint32_t position_x,
	uint32_t position_y,
	void *user_data)
{
	struct hyper_event_bus *event_bus = user_data;

	struct hyper_window_move_event window_move_event = {
		.position_x = position_x,
		.position_y = position_y,
	};
	struct hyper_event event_data = {
		.type = HYPER_EVENT_TYPE_WINDOW_MOVE,
		.window_move_event = window_move_event,
	};

	hyper_queue_push(&event_bus->event_list, &event_data);
}

static void hyper_window_resize_callback(
	struct hyper_window *window,
	uint32_t width,
	uint32_t height,
	void *user_data)
{
	struct hyper_event_bus *event_bus = user_data;

	struct hyper_window_resize_event window_resize_event = {
		.width = width,
		.height = height,
	};
	struct hyper_event event_data = {
		.type = HYPER_EVENT_TYPE_WINDOW_RESIZE,
		.window_resize_event = window_resize_event,
	};

	hyper_queue_push(&event_bus->event_list, &event_data);
}

static void hyper_window_framebuffer_resize_callback(
	struct hyper_window *window,
	uint32_t width,
	uint32_t height,
	void *user_data)
{
	struct hyper_event_bus *event_bus = user_data;

	struct hyper_window_framebuffer_resize_event
		window_framebuffer_resize_event = {
			.width = width,
			.height = height,
		};
	struct hyper_event event_data = {
		.type = HYPER_EVENT_TYPE_WINDOW_FRAMEBUFFER_RESIZE,
		.window_framebuffer_resize_event = window_framebuffer_resize_event,
	};

	hyper_queue_push(&event_bus->event_list, &event_data);
}

enum hyper_result hyper_event_bus_create(
	struct hyper_event_bus *event_bus,
	struct hyper_window *window)
{
	hyper_assert$(event_bus != NULL);
	hyper_assert$(window != NULL);

	if (
		hyper_queue_create(&event_bus->event_list, sizeof(struct hyper_event)) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$("Failed to create vector for 'event list'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

#define X(name, type)                                                         \
	if (                                                                        \
		hyper_vector_create(                                                      \
			&event_bus->name##_callbacks,                                           \
			sizeof(struct hyper_event_##name##_data)) != HYPER_RESULT_SUCCESS)      \
	{                                                                           \
		hyper_logger_error$("Failed to create vector for %s callbacks\n", #name); \
		return HYPER_RESULT_INITIALIZATION_FAILED;                                \
	}
	HYPER_EVENT_BUS_CALLBACKS
#undef X

	window->key_callback.callback = hyper_key_callback;
	window->key_callback.user_data = event_bus;
	window->key_type_callback.callback = hyper_key_type_callback;
	window->key_type_callback.user_data = event_bus;
	window->mouse_button_callback.callback = hyper_mouse_button_callback;
	window->mouse_button_callback.user_data = event_bus;
	window->mouse_move_callback.callback = hyper_mouse_move_callback;
	window->mouse_move_callback.user_data = event_bus;
	window->mouse_scroll_callback.callback = hyper_mouse_scroll_callback;
	window->mouse_scroll_callback.user_data = event_bus;
	window->window_close_callback.callback = hyper_window_close_callback;
	window->window_close_callback.user_data = event_bus;
	window->window_move_callback.callback = hyper_window_move_callback;
	window->window_move_callback.user_data = event_bus;
	window->window_resize_callback.callback = hyper_window_resize_callback;
	window->window_resize_callback.user_data = event_bus;
	window->window_framebuffer_resize_callback.callback =
		hyper_window_framebuffer_resize_callback;
	window->window_framebuffer_resize_callback.user_data = event_bus;

	return HYPER_RESULT_SUCCESS;
}

void hyper_event_bus_destroy(struct hyper_event_bus *event_bus)
{
	hyper_assert$(event_bus != NULL);

	hyper_queue_destroy(&event_bus->event_list);
#define X(name, type) hyper_vector_destroy(&event_bus->name##_callbacks);
	HYPER_EVENT_BUS_CALLBACKS
#undef X
}

void hyper_event_bus_process(struct hyper_event_bus *event_bus)
{
	if (event_bus->event_list.size == 0)
	{
		return;
	}

	struct hyper_event *event_data = hyper_queue_front(&event_bus->event_list);
	switch (event_data->type)
	{
#define X(name, type)                                                          \
	case type:                                                                   \
	{                                                                            \
		for (size_t i = 0; i < event_bus->name##_callbacks.size; ++i)              \
		{                                                                          \
			struct hyper_event_##name##_data *name##_data =                          \
				hyper_vector_get(&event_bus->name##_callbacks, i);                     \
			name##_data->callback(event_data->name##_event, name##_data->user_data); \
		}                                                                          \
		break;                                                                     \
	}
		HYPER_EVENT_BUS_CALLBACKS
#undef X
	default:
		break;
	}

	hyper_queue_pop(&event_bus->event_list);
}

#define X(name, type)                                                   \
	void hyper_register_##name##_callback(                                \
		struct hyper_event_bus *event_bus,                                  \
		hyper_event_##name##_callback_function name##_callback,             \
		void *user_data)                                                    \
	{                                                                     \
		struct hyper_event_##name##_data name##_data = {                    \
			.callback = name##_callback,                                      \
			.user_data = user_data,                                           \
		};                                                                  \
                                                                        \
		hyper_vector_push_back(&event_bus->name##_callbacks, &name##_data); \
	}

HYPER_EVENT_BUS_CALLBACKS
#undef X
