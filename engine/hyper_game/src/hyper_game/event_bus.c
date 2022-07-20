/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_game/event_bus.h"

#include "hyper_common/assertion.h"
#include "hyper_common/logger.h"
#include "hyper_game/event.h"

struct hyper_event_key_press_data
{
	hyper_event_key_press_callback_function callback;
	void *user_data;
};

struct hyper_event_key_release_data
{
	hyper_event_key_release_callback_function callback;
	void *user_data;
};

struct hyper_event_key_type_data
{
	hyper_event_key_type_callback_function callback;
	void *user_data;
};

struct hyper_event_mouse_button_press_data
{
	hyper_event_mouse_button_press_callback_function callback;
	void *user_data;
};

struct hyper_event_mouse_button_release_data
{
	hyper_event_mouse_button_release_callback_function callback;
	void *user_data;
};

struct hyper_event_mouse_move_data
{
	hyper_event_mouse_move_callback_function callback;
	void *user_data;
};

struct hyper_event_mouse_scroll_data
{
	hyper_event_mouse_scroll_callback_function callback;
	void *user_data;
};

struct hyper_event_window_close_data
{
	hyper_event_window_close_callback_function callback;
	void *user_data;
};

struct hyper_event_window_move_data
{
	hyper_event_window_move_callback_function callback;
	void *user_data;
};

struct hyper_event_window_resize_data
{
	hyper_event_window_resize_callback_function callback;
	void *user_data;
};

struct hyper_event_window_framebuffer_resize_data
{
	hyper_event_window_framebuffer_resize_callback_function callback;
	void *user_data;
};

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

	if (
		hyper_vector_create(
			&event_bus->key_press_callbacks,
			sizeof(struct hyper_event_key_press_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$("Failed to create vector for 'key press callbacks'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->key_release_callbacks,
			sizeof(struct hyper_event_key_release_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$(
			"Failed to create vector for 'key release callbacks'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->key_type_callbacks,
			sizeof(struct hyper_event_key_type_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$("Failed to create vector for 'key type callbacks'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->mouse_button_press_callbacks,
			sizeof(struct hyper_event_mouse_button_press_data)) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$(
			"Failed to create vector for 'mouse button press callbacks'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->mouse_button_release_callbacks,
			sizeof(struct hyper_event_mouse_button_release_data)) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$(
			"Failed to create vector for 'mouse button release callbacks'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->mouse_move_callbacks,
			sizeof(struct hyper_event_mouse_move_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$("Failed to create vector for 'mouse move callbacks'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->mouse_scroll_callbacks,
			sizeof(struct hyper_event_mouse_scroll_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$(
			"Failed to create vector for 'mouse scroll callbacks'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->window_close_callbacks,
			sizeof(struct hyper_event_window_close_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$(
			"Failed to create vector for 'window close callbacks'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->window_move_callbacks,
			sizeof(struct hyper_event_window_move_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$(
			"Failed to create vector for 'window move callbacks'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->window_resize_callbacks,
			sizeof(struct hyper_event_window_resize_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$(
			"Failed to create vector for 'window resize callbacks'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->window_framebuffer_resize_callbacks,
			sizeof(struct hyper_event_window_framebuffer_resize_data)) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$(
			"Failed to create vector for 'window framebuffer resize callbacks'\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

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
	hyper_vector_destroy(&event_bus->key_press_callbacks);
	hyper_vector_destroy(&event_bus->key_release_callbacks);
	hyper_vector_destroy(&event_bus->key_type_callbacks);
	hyper_vector_destroy(&event_bus->mouse_button_press_callbacks);
	hyper_vector_destroy(&event_bus->mouse_button_release_callbacks);
	hyper_vector_destroy(&event_bus->mouse_move_callbacks);
	hyper_vector_destroy(&event_bus->mouse_scroll_callbacks);
	hyper_vector_destroy(&event_bus->window_close_callbacks);
	hyper_vector_destroy(&event_bus->window_move_callbacks);
	hyper_vector_destroy(&event_bus->window_resize_callbacks);
	hyper_vector_destroy(&event_bus->window_framebuffer_resize_callbacks);
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
	case HYPER_EVENT_TYPE_KEY_PRESS:
	{
		for (size_t i = 0; i < event_bus->key_press_callbacks.size; ++i)
		{
			struct hyper_event_key_press_data *key_press_data =
				hyper_vector_get(&event_bus->key_press_callbacks, i);
			key_press_data->callback(
				event_data->key_press_event, key_press_data->user_data);
		}
		break;
	}
	case HYPER_EVENT_TYPE_KEY_RELEASE:
	{
		for (size_t i = 0; i < event_bus->key_release_callbacks.size; ++i)
		{
			struct hyper_event_key_release_data *lkey_release_data =
				hyper_vector_get(&event_bus->key_release_callbacks, i);
			lkey_release_data->callback(
				event_data->key_release_event, lkey_release_data->user_data);
		}
		break;
	}
	case HYPER_EVENT_TYPE_KEY_TYPE:
	{
		for (size_t i = 0; i < event_bus->key_type_callbacks.size; ++i)
		{
			struct hyper_event_key_type_data *key_type_data =
				hyper_vector_get(&event_bus->key_type_callbacks, i);
			key_type_data->callback(
				event_data->key_type_event, key_type_data->user_data);
		}
		break;
	}
	case HYPER_EVENT_TYPE_MOUSE_BUTTON_PRESS:
	{
		for (size_t i = 0; i < event_bus->mouse_button_press_callbacks.size; ++i)
		{
			struct hyper_event_mouse_button_press_data *mouse_button_press_data =
				hyper_vector_get(&event_bus->mouse_button_press_callbacks, i);
			mouse_button_press_data->callback(
				event_data->mouse_button_press_event,
				mouse_button_press_data->user_data);
		}
		break;
	}
	case HYPER_EVENT_TYPE_MOUSE_BUTTON_RELEASE:
	{
		for (size_t i = 0; i < event_bus->mouse_button_release_callbacks.size; ++i)
		{
			struct hyper_event_mouse_button_release_data *mouse_release_data =
				hyper_vector_get(&event_bus->mouse_button_release_callbacks, i);
			mouse_release_data->callback(
				event_data->mouse_button_release_event, mouse_release_data->user_data);
		}
		break;
	}
	case HYPER_EVENT_TYPE_MOUSE_MOVE:
	{
		for (size_t i = 0; i < event_bus->mouse_move_callbacks.size; ++i)
		{
			struct hyper_event_mouse_move_data *mouse_move_data =
				hyper_vector_get(&event_bus->mouse_move_callbacks, i);
			mouse_move_data->callback(
				event_data->mouse_move_event, mouse_move_data->user_data);
		}
		break;
	}
	case HYPER_EVENT_TYPE_MOUSE_SCROLL:
	{
		for (size_t i = 0; i < event_bus->mouse_scroll_callbacks.size; ++i)
		{
			struct hyper_event_mouse_scroll_data *mouse_scroll_data =
				hyper_vector_get(&event_bus->mouse_scroll_callbacks, i);
			mouse_scroll_data->callback(
				event_data->mouse_scroll_event, mouse_scroll_data->user_data);
		}
		break;
	}
	case HYPER_EVENT_TYPE_WINDOW_CLOSE:
	{
		for (size_t i = 0; i < event_bus->window_close_callbacks.size; ++i)
		{
			struct hyper_event_window_close_data *window_close_data =
				hyper_vector_get(&event_bus->window_close_callbacks, i);
			window_close_data->callback(
				event_data->window_close_event, window_close_data->user_data);
		}
		break;
	}
	case HYPER_EVENT_TYPE_WINDOW_RESIZE:
	{
		for (size_t i = 0; i < event_bus->window_resize_callbacks.size; ++i)
		{
			struct hyper_event_window_resize_data *window_resize_data =
				hyper_vector_get(&event_bus->window_resize_callbacks, i);
			window_resize_data->callback(
				event_data->window_resize_event, window_resize_data->user_data);
		}
		break;
	}
	case HYPER_EVENT_TYPE_WINDOW_FRAMEBUFFER_RESIZE:
	{
		for (size_t i = 0; i < event_bus->window_framebuffer_resize_callbacks.size;
				 ++i)
		{
			struct hyper_event_window_framebuffer_resize_data
				*window_framebuffer_resize_data =
					hyper_vector_get(&event_bus->window_framebuffer_resize_callbacks, i);
			window_framebuffer_resize_data->callback(
				event_data->window_framebuffer_resize_event,
				window_framebuffer_resize_data->user_data);
		}
		break;
	}
	case HYPER_EVENT_TYPE_WINDOW_MOVE:
	{
		for (size_t i = 0; i < event_bus->window_move_callbacks.size; ++i)
		{
			struct hyper_event_window_move_data *window_move_data =
				hyper_vector_get(&event_bus->window_move_callbacks, i);
			window_move_data->callback(
				event_data->window_move_event, window_move_data->user_data);
		}
		break;
	}
	default:
		break;
	}

	hyper_queue_pop(&event_bus->event_list);
}

void hyper_register_key_press_callback(
	struct hyper_event_bus *event_bus,
	hyper_event_key_press_callback_function key_press_callback,
	void *user_data)
{
	struct hyper_event_key_press_data key_press_data = {
		.callback = key_press_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->key_press_callbacks, &key_press_data);
}

void hyper_register_key_release_callback(
	struct hyper_event_bus *event_bus,
	hyper_event_key_release_callback_function key_release_callback,
	void *user_data)
{
	struct hyper_event_key_release_data key_release_data = {
		.callback = key_release_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->key_release_callbacks, &key_release_data);
}

void hyper_register_key_type_callback(
	struct hyper_event_bus *event_bus,
	hyper_event_key_type_callback_function key_type_callback,
	void *user_data)
{
	struct hyper_event_key_type_data key_type_data = {
		.callback = key_type_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->key_type_callbacks, &key_type_data);
}

void hyper_register_mouse_button_press_callback(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_button_press_callback_function mouse_button_press_callback,
	void *user_data)
{
	struct hyper_event_mouse_button_press_data mouse_button_press_data = {
		.callback = mouse_button_press_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(
		&event_bus->key_press_callbacks, &mouse_button_press_data);
}

void hyper_register_mouse_button_release_callback(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_button_release_callback_function
		mouse_button_release_callback,
	void *user_data)
{
	struct hyper_event_mouse_button_release_data mouse_button_release_data = {
		.callback = mouse_button_release_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(
		&event_bus->key_press_callbacks, &mouse_button_release_data);
}

void hyper_register_mouse_move_callback(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_move_callback_function mouse_move_callback,
	void *user_data)
{
	struct hyper_event_mouse_move_data mouse_move_data = {
		.callback = mouse_move_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->key_press_callbacks, &mouse_move_data);
}

void hyper_register_mouse_scroll_callback(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_scroll_callback_function mouse_scroll_callback,
	void *user_data)
{
	struct hyper_event_mouse_scroll_data mouse_scroll_data = {
		.callback = mouse_scroll_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->key_press_callbacks, &mouse_scroll_data);
}

void hyper_register_window_close_callback(
	struct hyper_event_bus *event_bus,
	hyper_event_window_close_callback_function window_close_callback,
	void *user_data)
{
	struct hyper_event_window_close_data window_close_data = {
		.callback = window_close_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(
		&event_bus->window_close_callbacks, &window_close_data);
}

void hyper_register_window_move_callback(
	struct hyper_event_bus *event_bus,
	hyper_event_window_move_callback_function window_move_callback,
	void *user_data)
{
	struct hyper_event_window_move_data window_move_data = {
		.callback = window_move_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->window_move_callbacks, &window_move_data);
}

void hyper_register_window_resize_callback(
	struct hyper_event_bus *event_bus,
	hyper_event_window_resize_callback_function window_resize_callback,
	void *user_data)
{
	struct hyper_event_window_resize_data window_resize_data = {
		.callback = window_resize_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(
		&event_bus->window_resize_callbacks, &window_resize_data);
}

void hyper_register_window_framebuffer_resize_callback(
	struct hyper_event_bus *event_bus,
	hyper_event_window_framebuffer_resize_callback_function
		window_framebuffer_resize_callback,
	void *user_data)
{
	struct hyper_event_window_framebuffer_resize_data
		window_framebuffer_resize_data = {
			.callback = window_framebuffer_resize_callback,
			.user_data = user_data,
		};

	hyper_vector_push_back(
		&event_bus->window_framebuffer_resize_callbacks,
		&window_framebuffer_resize_data);
}
