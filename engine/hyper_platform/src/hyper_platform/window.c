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

	return HYPER_RESULT_SUCCESS;
}

void hyper_window_destroy(struct hyper_window *window)
{
	hyper_assert$(window != NULL);

	glfwDestroyWindow(window->native_window);
	glfwTerminate();
}

void hyper_window_update(struct hyper_window *window)
{
	glfwPollEvents();
}
