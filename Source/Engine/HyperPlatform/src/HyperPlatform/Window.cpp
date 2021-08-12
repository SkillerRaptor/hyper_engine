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
	Window::Window(std::string title, int width, int height)
		: m_title(std::move(title))
		, m_width(width)
		, m_height(height)
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
		m_native_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
		if (m_native_window == nullptr)
		{
			glfwTerminate();
			return HyperCore::ConstructError::Incomplete;
		}
		
		return {};
	}
} // namespace HyperPlatform
