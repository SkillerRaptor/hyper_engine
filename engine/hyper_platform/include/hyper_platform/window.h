/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/library.h"
#include "hyper_common/result.h"

#include <stdint.h>

struct hyper_window;
enum hyper_key_action;
enum hyper_mouse_action;

typedef void (*hyper_key_callback_function)(
	struct hyper_window *window,
	enum hyper_key_action key_action,
	uint32_t key_code,
	void *user_data);
typedef void (*hyper_key_type_callback_function)(
	struct hyper_window *window,
	uint32_t key_code,
	void *user_data);

typedef void (*hyper_mouse_button_callback_function)(
	struct hyper_window *window,
	enum hyper_mouse_action mouse_action,
	uint32_t mouse_code,
	void *user_data);
typedef void (*hyper_mouse_move_callback_function)(
	struct hyper_window *window,
	float position_x,
	float position_y,
	void *user_data);
typedef void (*hyper_mouse_scroll_callback_function)(
	struct hyper_window *window,
	float offset_x,
	float offset_y,
	void *user_data);

typedef void (*hyper_window_close_callback_function)(
	struct hyper_window *window,
	void *user_data);
typedef void (*hyper_window_move_callback_function)(
	struct hyper_window *window,
	uint32_t position_x,
	uint32_t position_y,
	void *user_data);
typedef void (*hyper_window_resize_callback_function)(
	struct hyper_window *window,
	uint32_t width,
	uint32_t height,
	void *user_data);

struct hyper_key_callback_data
{
	hyper_key_callback_function callback;
	void *user_data;
};

struct hyper_key_type_callback_data
{
	hyper_key_type_callback_function callback;
	void *user_data;
};

struct hyper_mouse_button_callback_data
{
	hyper_mouse_button_callback_function callback;
	void *user_data;
};

struct hyper_mouse_move_callback_data
{
	hyper_mouse_move_callback_function callback;
	void *user_data;
};

struct hyper_mouse_scroll_callback_data
{
	hyper_mouse_scroll_callback_function callback;
	void *user_data;
};

struct hyper_window_close_callback_data
{
	hyper_window_close_callback_function callback;
	void *user_data;
};

struct hyper_window_move_callback_data
{
	hyper_window_move_callback_function callback;
	void *user_data;
};

struct hyper_window_resize_callback_data
{
	hyper_window_resize_callback_function callback;
	void *user_data;
};

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

	// TODO: Find a better way to carry the callbacks around
	struct hyper_key_callback_data key_callback;
	struct hyper_key_type_callback_data key_type_callback;
	struct hyper_mouse_button_callback_data mouse_button_callback;
	struct hyper_mouse_move_callback_data mouse_move_callback;
	struct hyper_mouse_scroll_callback_data mouse_scroll_callback;
	struct hyper_window_close_callback_data window_close_callback;
	struct hyper_window_move_callback_data window_move_callback;
	struct hyper_window_resize_callback_data window_resize_callback;

	void *native_window;
};

HYPER_API enum hyper_result hyper_window_create(
	struct hyper_window *window,
	const struct hyper_window_create_info *window_create_info);
HYPER_API void hyper_window_destroy(struct hyper_window *window);

HYPER_API void hyper_window_update(struct hyper_window *window);
