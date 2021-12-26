/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Platform/Window.hpp"

#include "HyperEngine/Logger.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace HyperEngine
{
	Window::Window(std::string title)
		: m_title(std::move(title))
	{
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
		: m_title(std::move(other.m_title))
		, m_window(std::exchange(other.m_window, nullptr))
	{
	}

	Window &Window::operator=(Window &&other) noexcept
	{
		m_title = std::move(other.m_title);
		m_window = std::exchange(other.m_window, nullptr);
		return *this;
	}

	Expected<void> Window::initialize(size_t width, size_t height)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(
			static_cast<int>(width),
			static_cast<int>(height),
			m_title.data(),
			nullptr,
			nullptr);
		if (m_window == nullptr)
		{
			return Error("failed to create window");
		}

		return {};
	}

	void Window::poll_events()
	{
		glfwPollEvents();
	}

	Expected<NonNullOwnPtr<VkSurfaceKHR>> Window::create_surface(
		const VkInstance &instance) const
	{
		VkSurfaceKHR surface = nullptr;
		const auto result =
			glfwCreateWindowSurface(instance, m_window, nullptr, &surface);
		if (result != VK_SUCCESS)
		{
			return Error("failed to create surface");
		}

		auto ptr = NonNullOwnPtr<VkSurfaceKHR>(
			surface,
			[&instance](VkSurfaceKHR handle)
			{
				vkDestroySurfaceKHR(instance, handle, nullptr);
			});
		return ptr;
	}

	void Window::set_title(std::string title)
	{
		m_title = std::move(title);
		glfwSetWindowTitle(m_window, m_title.c_str());
	}

	std::string Window::title() const
	{
		return m_title;
	}

	void Window::set_window_size(Vec2ui window_size)
	{
		glfwSetWindowSize(
			m_window,
			static_cast<int>(window_size.x),
			static_cast<int>(window_size.y));
	}

	Vec2ui Window::window_size() const
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

	Vec2ui Window::framebuffer_size() const
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

	Expected<NonNullOwnPtr<Window>> Window::create(
		std::string title,
		size_t width,
		size_t height)
	{
		assert(!title.empty() && "The title can't be empty");
		assert(width != 0 && "The width can't be 0");
		assert(height != 0 && "The height can't be 0");

		auto window = make_non_null_own<Window>(std::move(title));
		const auto result = window->initialize(width, height);
		if (result.is_error())
		{
			return result.error();
		}

		return window;
	}
} // namespace HyperEngine
