/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Platform/Window.hpp"

#include <GLFW/glfw3.h>

namespace HyperEngine
{
	Window::Window(
		const std::string &title,
		size_t width,
		size_t height,
		Error &error)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(
			static_cast<int>(width),
			static_cast<int>(height),
			title.c_str(),
			nullptr,
			nullptr);
		if (m_window == nullptr)
		{
			error = Error("failed to create glfw window");
			return;
		}
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Window::update()
	{
		glfwPollEvents();
	}

	Expected<std::unique_ptr<Window>> Window::create(
		const std::string &title,
		size_t width,
		size_t height)
	{
		Error error = Error::success();
		auto window = std::make_unique<Window>(title, width, height, error);
		if (error.is_error())
		{
			return error;
		}

		return window;
	}
} // namespace HyperEngine
