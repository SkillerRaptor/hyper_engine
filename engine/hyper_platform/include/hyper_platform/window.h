/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/library.h"
#include "hyper_common/result.h"

#include <stdbool.h>
#include <stdint.h>

struct hyper_window;

enum hyper_key_action
{
	HYPER_KEY_ACTION_NONE,
	HYPER_KEY_ACTION_PRESS,
	HYPER_KEY_ACTION_RELEASE,
	HYPER_KEY_ACTION_REPEAT,
};

enum hyper_mouse_action
{
	HYPER_MOUSE_ACTION_NONE,
	HYPER_MOUSE_ACTION_PRESS,
	HYPER_MOUSE_ACTION_RELEASE,
};

#define HYPER_WINDOW_CALLBACKS                                               \
	X(key, enum hyper_key_action key_action, uint32_t key_code)                \
	X(key_type, uint32_t key_code)                                             \
	X(mouse_button, enum hyper_mouse_action mouse_action, uint32_t mouse_code) \
	X(mouse_move, float position_x, float position_y)                          \
	X(mouse_scroll, float offset_x, float offset_y)                            \
	X(window_close, uint8_t unused)                                            \
	X(window_move, uint32_t position_x, uint32_t position_y)                   \
	X(window_resize, uint32_t width, uint32_t height)                          \
	X(window_framebuffer_resize, uint32_t width, uint32_t height)

#define X(name, ...)                                \
	typedef void (*hyper_##name##_callback_function)( \
		struct hyper_window * window, ##__VA_ARGS__, void *user_data);
HYPER_WINDOW_CALLBACKS
#undef X

#define X(name, ...)                           \
	struct hyper_##name##_callback_data          \
	{                                            \
		hyper_##name##_callback_function callback; \
		void *user_data;                           \
	};
HYPER_WINDOW_CALLBACKS
#undef X

struct hyper_window_create_info
{
	const char *title;
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
};

// TODO: Improve window identification for multiple windows
struct hyper_window
{
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;

	// TODO: Find a better way to carry the callbacks around
#define X(name, ...) struct hyper_##name##_callback_data name##_callback;
	HYPER_WINDOW_CALLBACKS
#undef X

	void *native_window;
};

HYPER_API enum hyper_result hyper_window_create(
	struct hyper_window *window,
	const struct hyper_window_create_info *window_create_info);
HYPER_API void hyper_window_destroy(struct hyper_window *window);

HYPER_API void hyper_window_update(struct hyper_window *window);

HYPER_API void hyper_window_get_required_extensions(
	struct hyper_window *window,
	const char ***extensions,
	uint32_t *extension_count);
HYPER_API bool hyper_window_create_window_surface(
	struct hyper_window *window,
	void *instance,
	void *surface);
