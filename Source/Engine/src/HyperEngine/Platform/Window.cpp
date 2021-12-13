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
		std::string_view title,
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
			title.data(),
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
		m_window = std::exchange(other.m_window, nullptr);
	}

	Window &Window::operator=(Window &&other) noexcept
	{
		m_window = std::exchange(other.m_window, nullptr);

		return *this;
	}

	void Window::update()
	{
		glfwPollEvents();
	}

	GLFWwindow *Window::native_window() const
	{
		return m_window;
	}

	Expected<Window *> Window::create(
		std::string_view title,
		size_t width,
		size_t height)
	{
		Error error = Error::success();
		auto *window = new Window(title, width, height, error);
		if (error.is_error())
		{
			delete window;
			return error;
		}

		return window;
	}
} // namespace HyperEngine
