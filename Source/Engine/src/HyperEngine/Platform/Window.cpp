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
		if (m_window != nullptr)
		{
			glfwDestroyWindow(m_window);
			glfwTerminate();
		}
	}

	Window::Window(Window &&other) noexcept
	{
		m_window = other.m_window;
		other.m_window = nullptr;
	}

	Window &Window::operator=(Window &&other) noexcept
	{
		m_window = other.m_window;

		other.m_window = nullptr;

		return *this;
	}

	void Window::update()
	{
		glfwPollEvents();
	}

	Expected<Window> Window::create(
		const std::string &title,
		size_t width,
		size_t height)
	{
		Error error = Error::success();
		Window window(title, width, height, error);
		if (error.is_error())
		{
			return error;
		}

		return window;
	}
} // namespace HyperEngine
