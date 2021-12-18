/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Math/Vector2.hpp"
#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <memory>
#include <string_view>

struct GLFWwindow;
using VkInstance = struct VkInstance_T *;
using VkSurfaceKHR = struct VkSurfaceKHR_T *;

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

		void poll_events();

		Expected<VkSurfaceKHR> create_surface(VkInstance instance) const;

		Vec2ui get_window_size() const;
		Vec2ui get_framebuffer_size() const;

		static Expected<Window *> create(
			std::string_view title,
			size_t width,
			size_t height);

	private:
		Window(std::string_view title, size_t width, size_t height, Error &error);

	private:
		GLFWwindow *m_window = nullptr;
	};
} // namespace HyperEngine
