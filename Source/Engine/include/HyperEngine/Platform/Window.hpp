/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <memory>
#include <string>

struct GLFWwindow;

namespace HyperEngine
{
	class Window
	{
	public:
		HYPERENGINE_NON_COPYABLE(Window);

	public:
		~Window();

		Window(Window &&other) noexcept;
		Window &operator=(Window &&other) noexcept;

		void update();

		static Expected<Window> create(
			const std::string &title,
			size_t width,
			size_t height);

	private:
		Window(const std::string &title, size_t width, size_t height, Error &error);

	private:
		GLFWwindow *m_window = nullptr;
	};
} // namespace HyperEngine
