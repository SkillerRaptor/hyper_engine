/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Platform/Window.hpp"

#define GLFW_INCLUDE_VULKAN
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
			error = Error("failed to create window");
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
		: m_window(std::exchange(other.m_window, nullptr))
	{
	}

	Window &Window::operator=(Window &&other) noexcept
	{
		m_window = std::exchange(other.m_window, nullptr);
		return *this;
	}

	void Window::poll_events()
	{
		glfwPollEvents();
	}

	Expected<VkSurfaceKHR> Window::create_surface(VkInstance instance) const
	{
		assert(instance != nullptr && "The instance can't be null");

		VkSurfaceKHR surface = nullptr;
		const auto result =
			glfwCreateWindowSurface(instance, m_window, nullptr, &surface);
		if (result != VK_SUCCESS)
		{
			return Error("failed to create surface");
		}

		return surface;
	}

	Vec2ui Window::get_window_size() const
	{
		int width = 0;
		int height = 0;
		glfwGetWindowSize(m_window, &width, &height);

		const Vec2ui size = {
			.x = static_cast<uint32_t>(width),
			.y = static_cast<uint32_t>(height),
		};
		return size;
	}

	Vec2ui Window::get_framebuffer_size() const
	{
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(m_window, &width, &height);

		const Vec2ui size = {
			.x = static_cast<uint32_t>(width),
			.y = static_cast<uint32_t>(height),
		};
		return size;
	}

	Expected<Window *> Window::create(
		std::string_view title,
		size_t width,
		size_t height)
	{
		assert(!title.empty() && "The title can't be empty");
		assert(width != 0 && "The width can't be 0");
		assert(height != 0 && "The height can't be 0");
		
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
