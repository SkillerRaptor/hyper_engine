/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Expected.hpp"

#include <memory>
#include <string>

struct GLFWwindow;

namespace HyperEngine
{
	class Window
	{
	public:
		Window(const std::string &title, size_t width, size_t height, Error &error);
		~Window();

		void update();

		static Expected<std::unique_ptr<Window>> create(
			const std::string &title,
			size_t width,
			size_t height);

	private:
		GLFWwindow *m_window = nullptr;
	};
} // namespace HyperEngine
