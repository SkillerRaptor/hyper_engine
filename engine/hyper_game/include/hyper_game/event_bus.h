/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/library.h"
#include "hyper_common/result.h"
#include "hyper_common/vector.h"
#include "hyper_game/key_events.h"
#include "hyper_game/mouse_events.h"
#include "hyper_game/window_events.h"
#include "hyper_platform/window.h"

typedef void (*hyper_event_key_press_function)(
	struct hyper_key_press_event key_press_event,
	void *user_data);

typedef void (*hyper_event_key_release_function)(
	struct hyper_key_release_event key_release_event,
	void *user_data);

typedef void (*hyper_event_key_type_function)(
	struct hyper_key_type_event key_type_event,
	void *user_data);

typedef void (*hyper_event_mouse_button_press_function)(
	struct hyper_mouse_button_press_event mouse_button_press_event,
	void *user_data);

typedef void (*hyper_event_mouse_button_release_function)(
	struct hyper_mouse_button_release_event mouse_button_release_event,
	void *user_data);

typedef void (*hyper_event_mouse_move_function)(
	struct hyper_mouse_move_event mouse_move_event,
	void *user_data);

typedef void (*hyper_event_mouse_scroll_function)(
	struct hyper_mouse_scroll_event mouse_scroll_event,
	void *user_data);

typedef void (*hyper_event_window_close_function)(
	struct hyper_window_close_event window_close_event,
	void *user_data);

typedef void (*hyper_event_window_move_function)(
	struct hyper_window_move_event window_move_event,
	void *user_data);

typedef void (*hyper_event_window_resize_function)(
	struct hyper_window_resize_event window_resize_event,
	void *user_data);

typedef void (*hyper_event_window_framebuffer_resize_function)(
	struct hyper_window_framebuffer_resize_event window_framebuffer_resize_event,
	void *user_data);

struct hyper_event_bus
{
	struct hyper_vector key_press_callbacks;
	struct hyper_vector key_release_callbacks;
	struct hyper_vector key_type_callbacks;
	struct hyper_vector mouse_button_press_callbacks;
	struct hyper_vector mouse_button_release_callbacks;
	struct hyper_vector mouse_move_callbacks;
	struct hyper_vector mouse_scroll_callbacks;
	struct hyper_vector window_close_callbacks;
	struct hyper_vector window_move_callbacks;
	struct hyper_vector window_resize_callbacks;
	struct hyper_vector window_framebuffer_resize_callbacks;
};

HYPER_API enum hyper_result hyper_event_bus_create(
	struct hyper_event_bus *event_bus,
	struct hyper_window *window);
HYPER_API void hyper_event_bus_destroy(struct hyper_event_bus *event_bus);

HYPER_API void hyper_event_bus_register_key_press(
	struct hyper_event_bus *event_bus,
	hyper_event_key_press_function key_press_callback,
	void *user_data);
HYPER_API void hyper_event_bus_register_key_release(
	struct hyper_event_bus *event_bus,
	hyper_event_key_release_function key_release_callback,
	void *user_data);
HYPER_API void hyper_event_bus_register_key_type(
	struct hyper_event_bus *event_bus,
	hyper_event_key_type_function key_type_callback,
	void *user_data);
HYPER_API void hyper_event_bus_register_mouse_button_press(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_button_press_function mouse_button_press_callback,
	void *user_data);
HYPER_API void hyper_event_bus_register_mouse_button_release(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_button_release_function mouse_button_release_callback,
	void *user_data);
HYPER_API void hyper_event_bus_register_mouse_move(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_move_function mouse_move_callback,
	void *user_data);
HYPER_API void hyper_event_bus_register_mouse_scroll(
	struct hyper_event_bus *event_bus,
	hyper_event_mouse_scroll_function mouse_scroll_callback,
	void *user_data);
HYPER_API void hyper_event_bus_register_window_close(
	struct hyper_event_bus *event_bus,
	hyper_event_window_close_function window_close_callback,
	void *user_data);
HYPER_API void hyper_event_bus_register_window_move(
	struct hyper_event_bus *event_bus,
	hyper_event_window_move_function window_move_callback,
	void *user_data);
HYPER_API void hyper_event_bus_register_window_resize(
	struct hyper_event_bus *event_bus,
	hyper_event_window_resize_function window_resize_callback,
	void *user_data);
HYPER_API void hyper_event_bus_register_window_framebuffer_resize(
	struct hyper_event_bus *event_bus,
	hyper_event_window_framebuffer_resize_function
		window_framebuffer_resize_callback,
	void *user_data);
