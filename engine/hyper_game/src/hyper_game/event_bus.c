/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_game/event_bus.h"

#include "hyper_common/assertion.h"
#include "hyper_common/core.h"
#include "hyper_common/logger.h"

struct hyper_event_key_press_data
{
	hyper_event_key_press_function callback;
	void *user_data;
};

struct hyper_event_key_release_data
{
	hyper_event_key_release_function callback;
	void *user_data;
};

struct hyper_event_key_type_data
{
	hyper_event_key_type_function callback;
	void *user_data;
};

struct hyper_event_mouse_button_press_data
{
	hyper_event_mouse_button_press_function callback;
	void *user_data;
};

struct hyper_event_mouse_button_release_data
{
	hyper_event_mouse_button_release_function callback;
	void *user_data;
};

struct hyper_event_mouse_move_data
{
	hyper_event_mouse_move_function callback;
	void *user_data;
};

struct hyper_event_mouse_scroll_data
{
	hyper_event_mouse_scroll_function callback;
	void *user_data;
};

struct hyper_event_window_close_data
{
	hyper_event_window_close_function callback;
	void *user_data;
};

struct hyper_event_window_move_data
{
	hyper_event_window_move_function callback;
	void *user_data;
};

struct hyper_event_window_resize_data
{
	hyper_event_window_resize_function callback;
	void *user_data;
};

struct hyper_event_window_framebuffer_resize_data
{
	hyper_event_window_framebuffer_resize_function callback;
	void *user_data;
};

#define HYPER_CALL_CALLBACK(event_name)                  \
	HYPER_VECTOR_FOREACH (                                 \
		event_bus->event_name##_callbacks,                   \
		i,                                                   \
		struct hyper_event_##event_name##_data,              \
		event_name##_data)                                   \
	{                                                      \
		event_name##_data->callback(                         \
			event_name##_event, event_name##_data->user_data); \
	}

static void hyper_key_callback(
	struct hyper_window *window,
	uint32_t key_code,
	enum hyper_key_action key_action,
	void *user_data)
{
	HYPER_UNUSED_VARIABLE(window);

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

		HYPER_CALL_CALLBACK(key_press);
		break;
	}
	case HYPER_KEY_ACTION_RELEASE:
	{
		struct hyper_key_release_event key_release_event = {
			.key_code = key_code,
		};

		HYPER_CALL_CALLBACK(key_release);
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
	HYPER_UNUSED_VARIABLE(window);

	struct hyper_event_bus *event_bus = user_data;

	struct hyper_key_type_event key_type_event = {
		.key_code = key_code,
	};

	HYPER_CALL_CALLBACK(key_type);
}

static void hyper_mouse_button_callback(
	struct hyper_window *window,
	uint32_t mouse_code,
	enum hyper_mouse_action mouse_action,
	void *user_data)
{
	HYPER_UNUSED_VARIABLE(window);

	struct hyper_event_bus *event_bus = user_data;

	switch (mouse_action)
	{
	case HYPER_MOUSE_ACTION_PRESS:
	{
		struct hyper_mouse_button_press_event mouse_button_press_event = {
			.mouse_code = mouse_code,
		};

		HYPER_CALL_CALLBACK(mouse_button_press);
		break;
	}
	case HYPER_MOUSE_ACTION_RELEASE:
	{
		struct hyper_mouse_button_release_event mouse_button_release_event = {
			.mouse_code = mouse_code,
		};

		HYPER_CALL_CALLBACK(mouse_button_release);
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
	HYPER_UNUSED_VARIABLE(window);

	struct hyper_event_bus *event_bus = user_data;

	struct hyper_mouse_move_event mouse_move_event = {
		.position_x = position_x,
		.position_y = position_y,
	};

	HYPER_CALL_CALLBACK(mouse_move);
}

static void hyper_mouse_scroll_callback(
	struct hyper_window *window,
	float offset_x,
	float offset_y,
	void *user_data)
{
	HYPER_UNUSED_VARIABLE(window);

	struct hyper_event_bus *event_bus = user_data;

	struct hyper_mouse_scroll_event mouse_scroll_event = {
		.offset_x = offset_x,
		.offset_y = offset_y,
	};

	HYPER_CALL_CALLBACK(mouse_scroll);
}

static void hyper_window_close_callback(
	struct hyper_window *window,
	void *user_data)
{
	HYPER_UNUSED_VARIABLE(window);

	struct hyper_event_bus *event_bus = user_data;

	struct hyper_window_close_event window_close_event = { 0 };

	HYPER_CALL_CALLBACK(window_close);
}

static void hyper_window_move_callback(
	struct hyper_window *window,
	uint32_t position_x,
	uint32_t position_y,
	void *user_data)
{
	HYPER_UNUSED_VARIABLE(window);

	struct hyper_event_bus *event_bus = user_data;

	struct hyper_window_move_event window_move_event = {
		.position_x = position_x,
		.position_y = position_y,
	};

	HYPER_CALL_CALLBACK(window_move);
}

static void hyper_window_resize_callback(
	struct hyper_window *window,
	uint32_t width,
	uint32_t height,
	void *user_data)
{
	HYPER_UNUSED_VARIABLE(window);

	struct hyper_event_bus *event_bus = user_data;

	struct hyper_window_resize_event window_resize_event = {
		.width = width,
		.height = height,
	};

	HYPER_CALL_CALLBACK(window_resize);
}

static void hyper_window_framebuffer_resize_callback(
	struct hyper_window *window,
	uint32_t width,
	uint32_t height,
	void *user_data)
{
	HYPER_UNUSED_VARIABLE(window);

	struct hyper_event_bus *event_bus = user_data;

	struct hyper_window_framebuffer_resize_event
		window_framebuffer_resize_event = {
			.width = width,
			.height = height,
		};

	HYPER_CALL_CALLBACK(window_framebuffer_resize);
}

enum hyper_result hyper_event_bus_create(
	struct hyper_event_bus *event_bus,
	struct hyper_window *window)
{
	HYPER_ASSERT(event_bus != NULL);
	HYPER_ASSERT(window != NULL);

	if (
		hyper_vector_create(
			&event_bus->key_press_callbacks,
			sizeof(struct hyper_event_key_press_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error(
			"Failed to create 'key press callbacks' vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->key_release_callbacks,
			sizeof(struct hyper_event_key_release_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error(
			"Failed to create 'key release callbacks' vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->key_type_callbacks,
			sizeof(struct hyper_event_key_type_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error(
			"Failed to create 'key type callbacks' vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->mouse_button_press_callbacks,
			sizeof(struct hyper_event_mouse_button_press_data)) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error(
			"Failed to create 'mouse button press callbacks' vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->mouse_button_release_callbacks,
			sizeof(struct hyper_event_mouse_button_release_data)) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error("Failed to create 'mouse button release callbacks' "
											 "vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->mouse_move_callbacks,
			sizeof(struct hyper_event_mouse_move_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error(
			"Failed to create 'mouse move callbacks' vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->mouse_scroll_callbacks,
			sizeof(struct hyper_event_mouse_scroll_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error(
			"Failed to create 'mouse scroll callbacks' vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->window_close_callbacks,
			sizeof(struct hyper_event_window_close_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error(
			"Failed to create 'window close callbacks' vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->window_move_callbacks,
			sizeof(struct hyper_event_window_move_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error(
			"Failed to create 'window move callbacks' vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->window_resize_callbacks,
			sizeof(struct hyper_event_window_resize_data)) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error(
			"Failed to create 'window resize callbacks' vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vector_create(
			&event_bus->window_framebuffer_resize_callbacks,
			sizeof(struct hyper_event_window_framebuffer_resize_data)) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error("Failed to create 'window framebuffer resize callbacks' "
											 "vector\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	window->key_function = hyper_key_callback;
	window->key_type_function = hyper_key_type_callback;
	window->mouse_button_function = hyper_mouse_button_callback;
	window->mouse_move_function = hyper_mouse_move_callback;
	window->mouse_scroll_function = hyper_mouse_scroll_callback;
	window->window_close_function = hyper_window_close_callback;
	window->window_move_function = hyper_window_move_callback;
	window->window_resize_function = hyper_window_resize_callback;
	window->window_framebuffer_resize_function =
		hyper_window_framebuffer_resize_callback;
	window->user_data = event_bus;

	hyper_logger_info("Successfully created event bus\n");

	return HYPER_RESULT_SUCCESS;
}

void hyper_event_bus_destroy(struct hyper_event_bus *event_bus)
{
	if (event_bus == NULL)
	{
		return;
	}

	hyper_vector_destroy(&event_bus->window_framebuffer_resize_callbacks);
	hyper_vector_destroy(&event_bus->window_resize_callbacks);
	hyper_vector_destroy(&event_bus->window_move_callbacks);
	hyper_vector_destroy(&event_bus->window_close_callbacks);
	hyper_vector_destroy(&event_bus->mouse_scroll_callbacks);
	hyper_vector_destroy(&event_bus->mouse_move_callbacks);
	hyper_vector_destroy(&event_bus->mouse_button_release_callbacks);
	hyper_vector_destroy(&event_bus->mouse_button_press_callbacks);
	hyper_vector_destroy(&event_bus->key_type_callbacks);
	hyper_vector_destroy(&event_bus->key_release_callbacks);
	hyper_vector_destroy(&event_bus->key_press_callbacks);

	hyper_logger_info("Successfully destroyed event bus\n");
}

void hyper_event_bus_register_key_press(
	struct hyper_event_bus *event_bus,
	hyper_event_key_press_function key_press_callback,
	void *user_data)
{
	struct hyper_event_key_press_data key_press_data = {
		.callback = key_press_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->key_press_callbacks, &key_press_data);
}

void hyper_event_bus_register_key_release(
	struct hyper_event_bus *event_bus,
	hyper_event_key_release_function key_release_callback,
	void *user_data)
{
	struct hyper_event_key_release_data key_release_data = {
		.callback = key_release_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->key_release_callbacks, &key_release_data);
}

void hyper_event_bus_register_key_type(
	struct hyper_event_bus *event_bus,
	hyper_event_key_type_function key_type_callback,
	void *user_data)
{
	struct hyper_event_key_type_data key_type_data = {
		.callback = key_type_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->key_type_callbacks, &key_type_data);
}

void hyper_event_bus_register_mouse_button_press(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_button_press_function mouse_button_press_callback,
	void *user_data)
{
	struct hyper_event_mouse_button_press_data mouse_button_press_data = {
		.callback = mouse_button_press_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(
		&event_bus->key_press_callbacks, &mouse_button_press_data);
}

void hyper_event_bus_register_mouse_button_release(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_button_release_function mouse_button_release_callback,
	void *user_data)
{
	struct hyper_event_mouse_button_release_data mouse_button_release_data = {
		.callback = mouse_button_release_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(
		&event_bus->key_press_callbacks, &mouse_button_release_data);
}

void hyper_event_bus_register_mouse_move(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_move_function mouse_move_callback,
	void *user_data)
{
	struct hyper_event_mouse_move_data mouse_move_data = {
		.callback = mouse_move_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->key_press_callbacks, &mouse_move_data);
}

void hyper_event_bus_register_mouse_scroll(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_scroll_function mouse_scroll_callback,
	void *user_data)
{
	struct hyper_event_mouse_scroll_data mouse_scroll_data = {
		.callback = mouse_scroll_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->key_press_callbacks, &mouse_scroll_data);
}

void hyper_event_bus_register_window_close(
	struct hyper_event_bus *event_bus,
	hyper_event_window_close_function window_close_callback,
	void *user_data)
{
	struct hyper_event_window_close_data window_close_data = {
		.callback = window_close_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(
		&event_bus->window_close_callbacks, &window_close_data);
}

void hyper_event_bus_register_window_move(
	struct hyper_event_bus *event_bus,
	hyper_event_window_move_function window_move_callback,
	void *user_data)
{
	struct hyper_event_window_move_data window_move_data = {
		.callback = window_move_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(&event_bus->window_move_callbacks, &window_move_data);
}

void hyper_event_bus_register_window_resize(
	struct hyper_event_bus *event_bus,
	hyper_event_window_resize_function window_resize_callback,
	void *user_data)
{
	struct hyper_event_window_resize_data window_resize_data = {
		.callback = window_resize_callback,
		.user_data = user_data,
	};

	hyper_vector_push_back(
		&event_bus->window_resize_callbacks, &window_resize_data);
}

void hyper_event_bus_register_window_framebuffer_resize(
	struct hyper_event_bus *event_bus,
	hyper_event_window_framebuffer_resize_function
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
