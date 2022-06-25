/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/platform/window.h"

#include "hyper_engine/utilities/assertion.h"
#include "hyper_engine/utilities/logger.h"

static void hyper_window_close_callback(GLFWwindow *native_window);
static void hyper_window_move_callback(
	GLFWwindow *native_window,
	int32_t x,
	int32_t y);
static void hyper_window_resize_callback(
	GLFWwindow *native_window,
	int32_t width,
	int32_t height);

enum hyper_result hyper_window_create(
	struct hyper_window *window,
	const struct hyper_window_create_info *window_create_info)
{
	assert$(window != NULL);
	assert$(window_create_info != NULL);

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
		logger_error("window: failed to create GLFW window");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	glfwSetWindowUserPointer(window->native_window, window);

	glfwSetWindowCloseCallback(
		window->native_window, hyper_window_close_callback);
	glfwSetWindowPosCallback(window->native_window, hyper_window_move_callback);
	glfwSetWindowSizeCallback(
		window->native_window, hyper_window_resize_callback);

	return HYPER_RESULT_SUCCESS;
}

void hyper_window_destroy(struct hyper_window *window)
{
	assert$(window != NULL);

	glfwDestroyWindow(window->native_window);
	glfwTerminate();
}

void hyper_window_update(struct hyper_window *window)
{
	assert$(window != NULL);

	glfwPollEvents();
}

static void hyper_window_close_callback(GLFWwindow *native_window)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	window->closed = true;

	// TODO: Add window close event
}

static void hyper_window_move_callback(
	GLFWwindow *native_window,
	int32_t x,
	int32_t y)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	window->x = x;
	window->y = y;

	// TODO: Add window move event
}

static void hyper_window_resize_callback(
	GLFWwindow *native_window,
	int32_t width,
	int32_t height)
{
	struct hyper_window *window = glfwGetWindowUserPointer(native_window);
	window->width = (size_t) width;
	window->height = (size_t) height;

	// TODO: Add window resize event
}
