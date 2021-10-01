/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/RenderingApi.hpp"

#include <cstddef>
#include <functional>
#include <string>

struct GLFWwindow;

namespace HyperEngine
{
	class CWindow
	{
	public:
		struct SDescription
		{
			std::string title;
			size_t width{ 0 };
			size_t height{ 0 };

			ERenderingApi rendering_api{ ERenderingApi::None };
			
			std::function<void(int32_t, bool)> key_pressed_callback{};
			std::function<void(int32_t)> key_released_callback{};
			std::function<void(float, float)> mouse_moved_callback{};
			std::function<void(float, float)> mouse_scrolled_callback{};
			std::function<void(int32_t)> mouse_button_pressed_callback{};
			std::function<void(int32_t)> mouse_button_released_callback{};
			std::function<void()> window_close_callback{};
			std::function<void(int32_t, int32_t)> window_resize_callback{};
			std::function<void(int32_t, int32_t)> window_framebuffer_resize_callback{};
			std::function<void()> window_focus_callback{};
			std::function<void()> window_lost_focus_callback{};
			std::function<void(int32_t, int32_t)> window_moved_callback{};
		};

	private:
		struct SInfo
		{
			size_t width{ 0 };
			size_t height{ 0 };
			
			std::function<void(int32_t, bool)> key_pressed_callback{};
			std::function<void(int32_t)> key_released_callback{};
			std::function<void(float, float)> mouse_moved_callback{};
			std::function<void(float, float)> mouse_scrolled_callback{};
			std::function<void(int32_t)> mouse_button_pressed_callback{};
			std::function<void(int32_t)> mouse_button_released_callback{};
			std::function<void()> window_close_callback{};
			std::function<void(int32_t, int32_t)> window_resize_callback{};
			std::function<void(int32_t, int32_t)> window_framebuffer_resize_callback{};
			std::function<void()> window_focus_callback{};
			std::function<void()> window_lost_focus_callback{};
			std::function<void(int32_t, int32_t)> window_moved_callback{};
		};

	public:
		auto create(const SDescription& description) -> bool;
		
		auto update() -> void;
		
		auto set_title(std::string title) -> void;
		auto title() const -> std::string;
		
		auto set_width(size_t width) -> void;
		auto width() const -> size_t;
		
		auto set_height(size_t height) -> void;
		auto height() const -> size_t;
		
		auto current_time() const -> float;

	private:
		auto create_callbacks() -> void;
		
	private:
		std::string m_title;
		SInfo m_info{};
		
		GLFWwindow* m_native_window{ nullptr };
	};
} // namespace HyperEngine
