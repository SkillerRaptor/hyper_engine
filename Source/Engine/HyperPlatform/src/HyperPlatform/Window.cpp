/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperPlatform/Window.hpp"

#include <HyperCore/Logger.hpp>

#include <GLFW/glfw3.h>

#include <utility>

namespace HyperPlatform
{
	Window::Window(std::string t_title, GraphicsApi t_graphics_api)
	{
		m_info.title = std::move(t_title);
		m_info.width = 1280;
		m_info.height = 720;
		m_graphics_api = t_graphics_api;

		initialize_callbacks();
	}

	Window::~Window()
	{
		auto successfully = true;

		if (m_native_window != nullptr)
		{
			glfwDestroyWindow(m_native_window);
		}
		else
		{
			successfully = false;
		}

		glfwTerminate();

		if (successfully)
		{
			HyperCore::Logger::info("Successfully destroyed window");
		}
	}

	auto Window::initialize() -> bool
	{
		if (glfwInit() == GLFW_FALSE)
		{
			HyperCore::Logger::fatal("Window::initialize(): Failed to initialize GLFW");
			return false;
		}

		HyperCore::Logger::debug("GLFW was initialized");

		switch (m_graphics_api)
		{
		case GraphicsApi::OpenGL:
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			break;
		default:
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			break;
		}

		m_native_window = glfwCreateWindow(m_info.width, m_info.height, m_info.title.c_str(), nullptr, nullptr);
		if (m_native_window == nullptr)
		{
			glfwTerminate();

			HyperCore::Logger::fatal("Window::initialize(): Failed to create GLFW window");
			return false;
		}

		glfwSetWindowUserPointer(m_native_window, &m_info);

		glfwSetKeyCallback(
			m_native_window,
			[](GLFWwindow* window, int32_t key, int32_t scan_code, int32_t action, int32_t mods)
			{
				HYPERENGINE_VARIABLE_NOT_USED(scan_code);
				HYPERENGINE_VARIABLE_NOT_USED(mods);

				auto window_info = reinterpret_cast<Info*>(glfwGetWindowUserPointer(window));

				switch (action)
				{
				case GLFW_PRESS:
					window_info->key_pressed_callback(key, false);
					break;
				case GLFW_RELEASE:
					window_info->key_released_callback(key);
					break;
				case GLFW_REPEAT:
					window_info->key_pressed_callback(key, true);
					break;
				default:
					break;
				}
			});

		glfwSetCursorPosCallback(
			m_native_window,
			[](GLFWwindow* window, double x, double y)
			{
				auto window_info = reinterpret_cast<Info*>(glfwGetWindowUserPointer(window));
				window_info->mouse_moved_callback(static_cast<float>(x), static_cast<float>(y));
			});

		glfwSetScrollCallback(
			m_native_window,
			[](GLFWwindow* window, double x, double y)
			{
				auto window_info = reinterpret_cast<Info*>(glfwGetWindowUserPointer(window));
				window_info->mouse_scrolled_callback(static_cast<float>(x), static_cast<float>(y));
			});

		glfwSetMouseButtonCallback(
			m_native_window,
			[](GLFWwindow* window, int32_t button, int32_t action, int)
			{
				auto window_info = reinterpret_cast<Info*>(glfwGetWindowUserPointer(window));

				switch (action)
				{
				case GLFW_PRESS:
					window_info->mouse_button_pressed_callback(button);
					break;
				case GLFW_RELEASE:
					window_info->mouse_button_released_callback(button);
					break;
				default:
					break;
				}
			});

		glfwSetWindowCloseCallback(
			m_native_window,
			[](GLFWwindow* window)
			{
				auto window_info = reinterpret_cast<Info*>(glfwGetWindowUserPointer(window));
				window_info->window_close_callback();
			});

		glfwSetWindowSizeCallback(
			m_native_window,
			[](GLFWwindow* window, int32_t width, int32_t height)
			{
				auto window_info = reinterpret_cast<Info*>(glfwGetWindowUserPointer(window));
				window_info->width = width;
				window_info->height = height;

				window_info->window_resize_callback(width, height);
			});

		glfwSetFramebufferSizeCallback(
			m_native_window,
			[](GLFWwindow* window, int32_t width, int32_t height)
			{
				auto window_info = reinterpret_cast<Info*>(glfwGetWindowUserPointer(window));
				window_info->width = width;
				window_info->height = height;

				window_info->window_framebuffer_resize_callback(width, height);
			});

		glfwSetWindowFocusCallback(
			m_native_window,
			[](GLFWwindow* window, int32_t focused)
			{
				auto window_info = reinterpret_cast<Info*>(glfwGetWindowUserPointer(window));

				if (focused == GLFW_TRUE)
				{
					window_info->window_focus_callback();
				}
				else if (focused == GLFW_FALSE)
				{
					window_info->window_lost_focus_callback();
				}
			});

		glfwSetWindowPosCallback(
			m_native_window,
			[](GLFWwindow* window, int32_t x, int32_t y)
			{
				auto window_info = reinterpret_cast<Info*>(glfwGetWindowUserPointer(window));
				window_info->window_moved_callback(x, y);
			});

		HyperCore::Logger::info("Successfully created window");

		return true;
	}

	auto Window::initialize_callbacks() -> void
	{
		m_info.key_pressed_callback = [](int32_t key, bool repeat)
		{
			HYPERENGINE_VARIABLE_NOT_USED(key);
			HYPERENGINE_VARIABLE_NOT_USED(repeat);
		};

		m_info.key_released_callback = [](int32_t key)
		{
			HYPERENGINE_VARIABLE_NOT_USED(key);
		};

		m_info.mouse_moved_callback = [](float x, float y)
		{
			HYPERENGINE_VARIABLE_NOT_USED(x);
			HYPERENGINE_VARIABLE_NOT_USED(y);
		};

		m_info.mouse_scrolled_callback = [](float x, float y)
		{
			HYPERENGINE_VARIABLE_NOT_USED(x);
			HYPERENGINE_VARIABLE_NOT_USED(y);
		};

		m_info.mouse_button_pressed_callback = [](int32_t button)
		{
			HYPERENGINE_VARIABLE_NOT_USED(button);
		};

		m_info.mouse_button_released_callback = [](int32_t button)
		{
			HYPERENGINE_VARIABLE_NOT_USED(button);
		};

		m_info.window_close_callback = []()
		{
		};

		m_info.window_resize_callback = [](int32_t width, int32_t height)
		{
			HYPERENGINE_VARIABLE_NOT_USED(width);
			HYPERENGINE_VARIABLE_NOT_USED(height);
		};

		m_info.window_framebuffer_resize_callback = [](int32_t width, int32_t height)
		{
			HYPERENGINE_VARIABLE_NOT_USED(width);
			HYPERENGINE_VARIABLE_NOT_USED(height);
		};

		m_info.window_focus_callback = []()
		{
		};

		m_info.window_lost_focus_callback = []()
		{
		};

		m_info.window_moved_callback = [](int32_t x, int32_t y)
		{
			HYPERENGINE_VARIABLE_NOT_USED(x);
			HYPERENGINE_VARIABLE_NOT_USED(y);
		};
	}

	auto Window::poll_events() const -> void
	{
		glfwPollEvents();
	}

	auto Window::time() const -> float
	{
		return static_cast<float>(glfwGetTime());
	}

	auto Window::set_title(std::string title) -> void
	{
		m_info.title = std::move(title);
		glfwSetWindowTitle(m_native_window, m_info.title.c_str());
	}

	auto Window::title() const -> std::string
	{
		return m_info.title;
	}

	auto Window::set_width(int32_t width) -> void
	{
		m_info.width = width;
		glfwSetWindowSize(m_native_window, m_info.width, m_info.height);
	}

	auto Window::width() const -> int32_t
	{
		return m_info.width;
	}

	auto Window::set_height(int32_t height) -> void
	{
		m_info.height = height;
		glfwSetWindowSize(m_native_window, m_info.width, m_info.height);
	}

	auto Window::height() const -> int32_t
	{
		return m_info.height;
	}

	auto Window::set_key_pressed_callback(const std::function<void(int32_t, bool)>& key_pressed_callback) -> void
	{
		m_info.key_pressed_callback = key_pressed_callback;
	}

	auto Window::set_key_released_callback(const std::function<void(int32_t)>& key_released_callback) -> void
	{
		m_info.key_released_callback = key_released_callback;
	}

	auto Window::set_mouse_moved_callback(const std::function<void(float, float)>& mouse_moved_callback) -> void
	{
		m_info.mouse_moved_callback = mouse_moved_callback;
	}

	auto Window::set_mouse_scrolled_callback(const std::function<void(float, float)>& mouse_scrolled_callback) -> void
	{
		m_info.mouse_scrolled_callback = mouse_scrolled_callback;
	}

	auto Window::set_mouse_button_pressed_callback(const std::function<void(int32_t)>& mouse_button_pressed_callback) -> void
	{
		m_info.mouse_button_pressed_callback = mouse_button_pressed_callback;
	}

	auto Window::set_mouse_button_released_callback(const std::function<void(int32_t)>& mouse_button_released_callback) -> void
	{
		m_info.mouse_button_released_callback = mouse_button_released_callback;
	}

	auto Window::set_window_close_callback(const std::function<void()>& window_close_callback) -> void
	{
		m_info.window_close_callback = window_close_callback;
	}

	auto Window::set_window_resize_callback(const std::function<void(int32_t, int32_t)>& window_resize_callback) -> void
	{
		m_info.window_resize_callback = window_resize_callback;
	}

	auto Window::set_window_framebuffer_resize_callback(const std::function<void(int32_t, int32_t)>& window_framebuffer_resize_callback) -> void
	{
		m_info.window_framebuffer_resize_callback = window_framebuffer_resize_callback;
	}

	auto Window::set_window_focus_callback(const std::function<void()>& window_focus_callback) -> void
	{
		m_info.window_focus_callback = window_focus_callback;
	}

	auto Window::set_window_lost_focus_callback(const std::function<void()>& window_lost_focus_callback) -> void
	{
		m_info.window_lost_focus_callback = window_lost_focus_callback;
	}

	auto Window::set_window_moved_callback(const std::function<void(int32_t, int32_t)>& window_moved_callback) -> void
	{
		m_info.window_moved_callback = window_moved_callback;
	}

	auto Window::graphics_api() const -> GraphicsApi
	{
		return m_graphics_api;
	}

	auto Window::native_window() const -> GLFWwindow*
	{
		return m_native_window;
	}
} // namespace HyperPlatform
