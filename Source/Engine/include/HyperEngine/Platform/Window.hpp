/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Math/Vector2.hpp"
#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/NonNullOwnPtr.hpp"

#include <string>

struct GLFWwindow;
using VkInstance = struct VkInstance_T *;
using VkSurfaceKHR = struct VkSurfaceKHR_T *;

namespace HyperEngine
{
	class Window
	{
	public:
		Window(std::string title);
		~Window();

		Window(Window &&other) noexcept;
		Window &operator=(Window &&other) noexcept;

		Expected<void> initialize(size_t width, size_t height);
		void poll_events();

		Expected<NonNullOwnPtr<VkSurfaceKHR>> create_surface(
			const VkInstance &instance) const;

		void set_title(std::string title);
		std::string title() const;

		void set_window_size(Vec2ui window_size);
		Vec2ui window_size() const;
		Vec2ui framebuffer_size() const;

		static Expected<NonNullOwnPtr<Window>> create(
			std::string title,
			size_t width,
			size_t height);

	private:
		std::string m_title;

		GLFWwindow *m_window = nullptr;
	};
} // namespace HyperEngine
