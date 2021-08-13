/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperPlatform/GraphicsApi.hpp"

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>

#include <string>

struct GLFWwindow;

namespace HyperPlatform
{
	class Window
	{
	public:
		Window(std::string title, int width, int height, GraphicsApi graphics_api);
		~Window();

		auto initialize() -> HyperCore::Result<void, HyperCore::ConstructError>;

		auto poll_events() const -> void;

	private:
		std::string m_title;
		int m_width{ 0 };
		int m_height{ 0 };
		GraphicsApi m_graphics_api{ GraphicsApi::OpenGL33 };

		GLFWwindow* m_native_window{ nullptr };
	};
} // namespace HyperPlatform
