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
	Window::Window(std::string title, int width, int height, GraphicsApi graphics_api)
		: m_title(std::move(title))
		, m_width(width)
		, m_height(height)
		, m_graphics_api(graphics_api)
	{
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

		m_native_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
		if (m_native_window == nullptr)
		{
			glfwTerminate();
			return HyperCore::ConstructError::Incomplete;
		}

		switch (m_graphics_api)
		{
		case GraphicsApi::OpenGL33:
		case GraphicsApi::OpenGL46:
			glfwMakeContextCurrent(m_native_window);
		default:
			break;
		}

		return {};
	}

	auto Window::poll_events() const -> void
	{
		glfwPollEvents();
	}
} // namespace HyperPlatform
