/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_platform/window.h"

#include "hyper_common/assertion.h"
#include "hyper_common/logger.h"
#include "hyper_common/prerequisites.h"

#define GLFW_INCLUDE_VULKAN
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
	hyper_unused_variable$(scan_code);
	hyper_unused_variable$(mods);

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
			window, key_action, (uint32_t) key_code, window->key_callback.user_data);
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
	hyper_unused_variable$(mods);

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
			window,
			mouse_action,
			(uint32_t) button,
			window->mouse_button_callback.user_data);
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
			window, 0, window->window_close_callback.user_data);
	}
}

static void hyper_window_move_callback(
	GLFWwindow *native_window,
	int position_x,
	int position_y)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	window->x = (uint32_t) position_x;
	window->y = (uint32_t) position_y;

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
	window->width = (uint32_t) width;
	window->height = (uint32_t) height;

	if (window->window_resize_callback.callback != NULL)
	{
		window->window_resize_callback.callback(
			window,
			(uint32_t) width,
			(uint32_t) height,
			window->window_resize_callback.user_data);
	}
}

static void hyper_window_framebuffer_resize_callback(
	GLFWwindow *native_window,
	int width,
	int height)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	window->framebuffer_width = (uint32_t) width;
	window->framebuffer_height = (uint32_t) height;

	if (window->window_framebuffer_resize_callback.callback != NULL)
	{
		window->window_framebuffer_resize_callback.callback(
			window,
			(uint32_t) width,
			(uint32_t) height,
			window->window_framebuffer_resize_callback.user_data);
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
		(int) window_create_info->width,
		(int) window_create_info->height,
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
	glfwSetFramebufferSizeCallback(
		window->native_window, hyper_window_framebuffer_resize_callback);

	// TODO: Improve setting initial values
	int x = 0;
	int y = 0;
	glfwGetWindowPos(window->native_window, &x, &y);
	window->x = (uint32_t) x;
	window->y = (uint32_t) y;

	int width = 0;
	int height = 0;
	glfwGetWindowSize(window->native_window, &width, &height);
	window->width = (uint32_t) width;
	window->height = (uint32_t) height;

	int framebuffer_width = 0;
	int framebuffer_height = 0;
	glfwGetFramebufferSize(
		window->native_window, &framebuffer_width, &framebuffer_height);
	window->framebuffer_width = (uint32_t) framebuffer_width;
	window->framebuffer_height = (uint32_t) framebuffer_height;

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
	hyper_unused_debug_variable$(window);

	hyper_assert$(window != NULL);

	glfwPollEvents();
}

void hyper_window_get_required_extensions(
	struct hyper_window *window,
	const char ***extensions,
	uint32_t *extension_count)
{
	hyper_unused_variable$(window);

	*extensions = glfwGetRequiredInstanceExtensions(extension_count);
}

bool hyper_window_create_window_surface(
	struct hyper_window *window,
	void *instance,
	void *surface)
{
	if (
		glfwCreateWindowSurface(
			instance, window->native_window, NULL, (VkSurfaceKHR *) surface) !=
		VK_SUCCESS)
	{
		return false;
	}

	return true;
}
