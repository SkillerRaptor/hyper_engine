/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Platform/Window.hpp"

#include "HyperEngine/Core/Assertion.hpp"

#include <GLFW/glfw3.h>

namespace HyperEngine
{
	auto CWindow::create(const CWindow::SDescription& description) -> bool
	{
		HYPERENGINE_ASSERT_IS_FALSE(description.title.empty());
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.width, 0);
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.height, 0);
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.rendering_api, ERenderingApi::None);

		m_title = description.title;
		m_info.width = description.width;
		m_info.height = description.height;
		m_info.key_pressed_callback = description.key_pressed_callback;
		m_info.key_released_callback = description.key_released_callback;
		m_info.mouse_moved_callback = description.mouse_moved_callback;
		m_info.mouse_scrolled_callback = description.mouse_scrolled_callback;
		m_info.mouse_button_pressed_callback = description.mouse_button_pressed_callback;
		m_info.mouse_button_released_callback = description.mouse_button_released_callback;
		m_info.window_close_callback = description.window_close_callback;
		m_info.window_resize_callback = description.window_resize_callback;
		m_info.window_framebuffer_resize_callback = description.window_framebuffer_resize_callback;
		m_info.window_focus_callback = description.window_focus_callback;
		m_info.window_lost_focus_callback = description.window_lost_focus_callback;
		m_info.window_moved_callback = description.window_moved_callback;

		glfwInit();

		switch (description.rendering_api)
		{
#if HYPERENGINE_BUILD_OPENGL
		case ERenderingApi::OpenGL:
			CLogger::fatal("CWindow::create(): The OpenGL window is not implemented yet");
			return false;
#endif
#if HYPERENGINE_BUILD_VULKAN
		case ERenderingApi::Vulkan:
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			break;
#endif
		default:
			CLogger::fatal("CWindow::create(): The specified rendering api is not available");
			return false;
		}

		m_native_window = glfwCreateWindow(
			static_cast<int32_t>(m_info.width),
			static_cast<int32_t>(m_info.height),
			m_title.c_str(),
			nullptr,
			nullptr);
		if (m_native_window == nullptr)
		{
			CLogger::fatal("CWindow::create(): Failed to create GLFW window");
			return false;
		}

		glfwSetWindowUserPointer(m_native_window, &m_info);
		create_callbacks();

		return true;
	}

	auto CWindow::create_callbacks() -> void
	{
		glfwSetCursorPosCallback(
			m_native_window,
			[](GLFWwindow* window, double x, double y)
			{
				const SInfo* window_info = reinterpret_cast<SInfo*>(glfwGetWindowUserPointer(window));
				if (window_info->mouse_moved_callback != nullptr)
				{
					window_info->mouse_moved_callback(static_cast<float>(x), static_cast<float>(y));
				}
			});

		glfwSetScrollCallback(
			m_native_window,
			[](GLFWwindow* window, double x, double y)
			{
				const SInfo* window_info = reinterpret_cast<SInfo*>(glfwGetWindowUserPointer(window));
				if (window_info->mouse_scrolled_callback != nullptr)
				{
					window_info->mouse_scrolled_callback(static_cast<float>(x), static_cast<float>(y));
				}
			});

		glfwSetMouseButtonCallback(
			m_native_window,
			[](GLFWwindow* window, int32_t button, int32_t action, int)
			{
				const SInfo* window_info = reinterpret_cast<SInfo*>(glfwGetWindowUserPointer(window));

				switch (action)
				{
				case GLFW_PRESS:
					if (window_info->mouse_button_pressed_callback != nullptr)
					{
						window_info->mouse_button_pressed_callback(button);
					}
					break;
				case GLFW_RELEASE:
					if (window_info->mouse_button_released_callback != nullptr)
					{
						window_info->mouse_button_released_callback(button);
					}
					break;
				default:
					break;
				}
			});

		glfwSetWindowCloseCallback(
			m_native_window,
			[](GLFWwindow* window)
			{
				const SInfo* window_info = reinterpret_cast<SInfo*>(glfwGetWindowUserPointer(window));
				if (window_info->window_close_callback != nullptr)
				{
					window_info->window_close_callback();
				}
			});

		glfwSetWindowSizeCallback(
			m_native_window,
			[](GLFWwindow* window, int32_t width, int32_t height)
			{
				SInfo* window_info = reinterpret_cast<SInfo*>(glfwGetWindowUserPointer(window));
				window_info->width = static_cast<size_t>(width);
				window_info->height = static_cast<size_t>(height);

				if (window_info->window_resize_callback != nullptr)
				{
					window_info->window_resize_callback(width, height);
				}
			});

		glfwSetFramebufferSizeCallback(
			m_native_window,
			[](GLFWwindow* window, int32_t width, int32_t height)
			{
				const SInfo* window_info = reinterpret_cast<SInfo*>(glfwGetWindowUserPointer(window));

				if (window_info->window_framebuffer_resize_callback != nullptr)
				{
					window_info->window_framebuffer_resize_callback(width, height);
				}
			});

		glfwSetWindowFocusCallback(
			m_native_window,
			[](GLFWwindow* window, int32_t focused)
			{
				const SInfo* window_info = reinterpret_cast<SInfo*>(glfwGetWindowUserPointer(window));

				if (focused == GLFW_TRUE)
				{
					if (window_info->window_focus_callback != nullptr)
					{
						window_info->window_focus_callback();
					}
				}
				else if (focused == GLFW_FALSE)
				{
					if (window_info->window_lost_focus_callback != nullptr)
					{
						window_info->window_lost_focus_callback();
					}
				}
			});

		glfwSetWindowPosCallback(
			m_native_window,
			[](GLFWwindow* window, int32_t x, int32_t y)
			{
				const SInfo* window_info = reinterpret_cast<SInfo*>(glfwGetWindowUserPointer(window));
				if (window_info->window_moved_callback != nullptr)
				{
					window_info->window_moved_callback(x, y);
				}
			});
	}

	auto CWindow::update() -> void
	{
		glfwPollEvents();
	}

	auto CWindow::set_title(std::string title) -> void
	{
		m_title = std::move(title);
		glfwSetWindowTitle(m_native_window, m_title.c_str());
	}

	auto CWindow::title() const -> std::string
	{
		return m_title;
	}

	auto CWindow::set_width(size_t width) -> void
	{
		m_info.width = width;
		glfwSetWindowSize(m_native_window, static_cast<int32_t>(m_info.width), static_cast<int32_t>(m_info.height));
	}

	auto CWindow::width() const noexcept -> size_t
	{
		return m_info.width;
	}

	auto CWindow::set_height(size_t height) -> void
	{
		m_info.height = height;
		glfwSetWindowSize(m_native_window, static_cast<int32_t>(m_info.width), static_cast<int32_t>(m_info.height));
	}

	auto CWindow::height() const noexcept -> size_t
	{
		return m_info.height;
	}

	auto CWindow::native_window() noexcept -> GLFWwindow*
	{
		return m_native_window;
	}

	auto CWindow::current_time() const noexcept -> float
	{
		return static_cast<float>(glfwGetTime());
	}
} // namespace HyperEngine
