/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_engine/utilities/result.h"

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stddef.h>

struct hyper_window
{
	const char *title;
	int32_t x;
	int32_t y;
	size_t width;
	size_t height;

	bool closed;

	GLFWwindow *native_window;
};

struct hyper_window_create_info
{
	const char *title;
	size_t width;
	size_t height;
};

enum hyper_result hyper_window_create(
	struct hyper_window *window,
	const struct hyper_window_create_info *window_create_info);
void hyper_window_destroy(struct hyper_window *window);

void hyper_window_update(struct hyper_window *window);
