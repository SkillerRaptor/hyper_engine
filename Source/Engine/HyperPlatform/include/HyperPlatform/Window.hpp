/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>

#include <string>

struct GLFWwindow;

namespace HyperPlatform
{
	class Window
	{
	public:
		Window(std::string title, int width, int height);
		~Window();
		
		auto initialize() -> HyperCore::Result<void, HyperCore::ConstructError>;

	private:
		std::string m_title;
		int m_width{ 0 };
		int m_height{ 0 };

		GLFWwindow* m_native_window{ nullptr };
	};
} // namespace HyperPlatform
