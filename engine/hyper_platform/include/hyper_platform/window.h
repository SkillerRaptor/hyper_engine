/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/core.h"
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

typedef void (*hyper_key_function)(
	struct hyper_window *window,
	uint32_t key_code,
	enum hyper_key_action key_action,
	void *user_data);

typedef void (*hyper_key_type_function)(
	struct hyper_window *window,
	uint32_t key_code,
	void *user_data);

typedef void (*hyper_mouse_button_function)(
	struct hyper_window *window,
	uint32_t mouse_code,
	enum hyper_mouse_action mouse_action,
	void *user_data);

typedef void (*hyper_mouse_move_function)(
	struct hyper_window *window,
	float position_x,
	float position_y,
	void *user_data);

typedef void (*hyper_mouse_scroll_function)(
	struct hyper_window *window,
	float offset_x,
	float offset_y,
	void *user_data);

typedef void (
	*hyper_window_close_function)(struct hyper_window *window, void *user_data);

typedef void (*hyper_window_move_function)(
	struct hyper_window *window,
	uint32_t position_x,
	uint32_t position_y,
	void *user_data);

typedef void (*hyper_window_resize_function)(
	struct hyper_window *window,
	uint32_t width,
	uint32_t height,
	void *user_data);

typedef void (*hyper_window_framebuffer_resize_function)(
	struct hyper_window *window,
	uint32_t width,
	uint32_t height,
	void *user_data);

struct hyper_window_create_info
{
	const char *title;
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
};

struct hyper_window
{
	const char *title;
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;

	hyper_key_function key_function;
	hyper_key_type_function key_type_function;
	hyper_mouse_button_function mouse_button_function;
	hyper_mouse_move_function mouse_move_function;
	hyper_mouse_scroll_function mouse_scroll_function;
	hyper_window_close_function window_close_function;
	hyper_window_move_function window_move_function;
	hyper_window_resize_function window_resize_function;
	hyper_window_framebuffer_resize_function window_framebuffer_resize_function;
	void *user_data;

	void *native_window;
};

HYPER_API enum hyper_result hyper_window_create(
	struct hyper_window *window,
	const struct hyper_window_create_info *window_create_info);
HYPER_API void hyper_window_destroy(struct hyper_window *window);

HYPER_API void hyper_window_poll_events(void);

HYPER_API bool hyper_window_create_window_surface(
	struct hyper_window *window,
	void *HYPER_RESTRICT instance,
	void *HYPER_RESTRICT surface);
HYPER_API void hyper_window_get_required_extensions(
	const char ***extensions,
	uint32_t *extension_count);
