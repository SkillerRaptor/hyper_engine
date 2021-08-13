/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperPlatform/Window.hpp"

#include <GLFW/glfw3.h>

#include <utility>

namespace HyperPlatform
{
	Window::Window(std::string title, int width, int height, GraphicsApi graphics_api, HyperCore::EventManager& event_manager)
		: m_graphics_api(graphics_api)
	{
		m_info.title = std::move(title);
		m_info.width = width;
		m_info.height = height;
		m_info.event_manager = &event_manager;
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_native_window);
		glfwTerminate();
	}

	auto Window::initialize() -> HyperCore::Result<void, HyperCore::ConstructError>
	{
		glfwInit();

		switch (m_graphics_api)
		{
		case GraphicsApi::OpenGL33:
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			break;
		case GraphicsApi::OpenGL46:
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			break;
		default:
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			break;
		}

		m_native_window = glfwCreateWindow(m_info.width, m_info.height, m_info.title.c_str(), nullptr, nullptr);
		if (m_native_window == nullptr)
		{
			glfwTerminate();
			return HyperCore::ConstructError::Incomplete;
		}

		glfwSetWindowUserPointer(m_native_window, &m_info);

		glfwSetWindowSizeCallback(
			m_native_window,
			[](GLFWwindow* window, int width, int height)
			{
				auto window_info = reinterpret_cast<Info*>(glfwGetWindowUserPointer(window));
				window_info->width = width;
				window_info->height = height;
				
				window_info->event_manager->invoke<HyperCore::WindowResizeEvent>(width, height);
			});

		glfwSetWindowCloseCallback(
			m_native_window,
			[](GLFWwindow* window)
			{
				auto window_info = reinterpret_cast<Info*>(glfwGetWindowUserPointer(window));
				window_info->event_manager->invoke<HyperCore::WindowCloseEvent>();
			});

		return {};
	}

	auto Window::poll_events() const -> void
	{
		glfwPollEvents();
	}
	
	auto Window::native_window() const -> GLFWwindow*
	{
		return m_native_window;
	}
} // namespace HyperPlatform
