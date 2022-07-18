/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_platform/window.h"

#include "hyper_common/assertion.h"
#include "hyper_common/logger.h"

#include <GLFW/glfw3.h>
#include <stddef.h>

static uint16_t s_window_count = 0;

static void hyper_key_callback(
	GLFWwindow *native_window,
	int key_code,
	int scan_code,
	int action,
	int mods)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	if (window->key_callback.callback != NULL)
	{
		enum hyper_key_action key_action = HYPER_KEY_ACTION_NONE;
		switch (action)
		{
		case GLFW_PRESS:
			key_action = HYPER_KEY_ACTION_PRESS;
			break;
		case GLFW_RELEASE:
			key_action = HYPER_KEY_ACTION_RELEASE;
			break;
		case GLFW_REPEAT:
			key_action = HYPER_KEY_ACTION_REPEAT;
			break;
		default:
			break;
		}

		window->key_callback.callback(
			window, key_action, key_code, window->key_callback.user_data);
	}
}

static void hyper_key_type_callback(
	GLFWwindow *native_window,
	unsigned int key_code)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	if (window->key_type_callback.callback != NULL)
	{
		window->key_type_callback.callback(
			window, key_code, window->key_type_callback.user_data);
	}
}

static void hyper_mouse_button_callback(
	GLFWwindow *native_window,
	int button,
	int action,
	int mods)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	if (window->mouse_button_callback.callback != NULL)
	{
		enum hyper_mouse_action mouse_action = HYPER_MOUSE_ACTION_NONE;
		switch (action)
		{
		case GLFW_PRESS:
			mouse_action = HYPER_MOUSE_ACTION_PRESS;
			break;
		case GLFW_RELEASE:
			mouse_action = HYPER_MOUSE_ACTION_RELEASE;
			break;
		default:
			break;
		}

		window->mouse_button_callback.callback(
			window, mouse_action, button, window->mouse_button_callback.user_data);
	}
}

static void hyper_mouse_scroll_callback(
	GLFWwindow *native_window,
	double offset_x,
	double offset_y)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	if (window->mouse_scroll_callback.callback != NULL)
	{
		window->mouse_scroll_callback.callback(
			window,
			(float) offset_x,
			(float) offset_y,
			window->mouse_scroll_callback.user_data);
	}
}

static void hyper_mouse_move_callback(
	GLFWwindow *native_window,
	double position_x,
	double position_y)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	if (window->mouse_move_callback.callback != NULL)
	{
		window->mouse_move_callback.callback(
			window,
			(float) position_x,
			(float) position_y,
			window->mouse_move_callback.user_data);
	}
}

static void hyper_window_close_callback(GLFWwindow *native_window)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	if (window->window_close_callback.callback != NULL)
	{
		window->window_close_callback.callback(
			window, window->window_close_callback.user_data);
	}
}

static void hyper_window_move_callback(
	GLFWwindow *native_window,
	int position_x,
	int position_y)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	window->x = position_x;
	window->y = position_y;

	if (window->window_move_callback.callback != NULL)
	{
		window->window_move_callback.callback(
			window,
			(uint32_t) position_x,
			(uint32_t) position_y,
			window->window_move_callback.user_data);
	}
}

static void hyper_window_resize_callback(
	GLFWwindow *native_window,
	int width,
	int height)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	window->width = width;
	window->height = height;

	if (window->window_resize_callback.callback != NULL)
	{
		window->window_resize_callback.callback(
			window,
			(uint32_t) width,
			(uint32_t) height,
			window->window_resize_callback.user_data);
	}
}

enum hyper_result hyper_window_create(
	struct hyper_window *window,
	const struct hyper_window_create_info *window_create_info)
{
	hyper_assert$(window != NULL);
	hyper_assert$(window_create_info != NULL);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window->native_window = glfwCreateWindow(
		window_create_info->width,
		window_create_info->height,
		window_create_info->title,
		NULL,
		NULL);
	if (window->native_window == NULL)
	{
		hyper_logger_error$("Failed to create window\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	glfwSetWindowUserPointer(window->native_window, window);

	glfwSetKeyCallback(window->native_window, hyper_key_callback);
	glfwSetCharCallback(window->native_window, hyper_key_type_callback);
	glfwSetMouseButtonCallback(
		window->native_window, hyper_mouse_button_callback);
	glfwSetScrollCallback(window->native_window, hyper_mouse_scroll_callback);
	glfwSetCursorPosCallback(window->native_window, hyper_mouse_move_callback);
	glfwSetWindowCloseCallback(
		window->native_window, hyper_window_close_callback);
	glfwSetWindowPosCallback(window->native_window, hyper_window_move_callback);
	glfwSetWindowSizeCallback(
		window->native_window, hyper_window_resize_callback);

	++s_window_count;

	return HYPER_RESULT_SUCCESS;
}

void hyper_window_destroy(struct hyper_window *window)
{
	hyper_assert$(window != NULL);

	--s_window_count;

	glfwDestroyWindow(window->native_window);

	if (s_window_count == 0)
	{
		glfwTerminate();
	}
}

void hyper_window_update(struct hyper_window *window)
{
	hyper_assert$(window != NULL);

	glfwPollEvents();
}
