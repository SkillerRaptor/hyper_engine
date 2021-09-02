/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperPlatform/GraphicsApi.hpp"

#include <cstdint>
#include <functional>
#include <string>

struct GLFWwindow;

namespace HyperPlatform
{
	class Window
	{
	private:
		struct Info
		{
			std::string title;
			int32_t width{ 0 };
			int32_t height{ 0 };

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
		Window(std::string t_title, GraphicsApi t_graphics_api);
		~Window();

		auto initialize() -> bool;

		auto poll_events() const -> void;
		auto time() const -> float;

		auto set_title(std::string title) -> void;
		auto title() const -> std::string;

		auto set_width(int32_t width) -> void;
		auto width() const -> int32_t;

		auto set_height(int32_t height) -> void;
		auto height() const -> int32_t;

		auto set_key_pressed_callback(const std::function<void(int32_t, bool)>& key_pressed_callback) -> void;
		auto set_key_released_callback(const std::function<void(int32_t)>& key_released_callback) -> void;
		auto set_mouse_moved_callback(const std::function<void(float, float)>& mouse_moved_callback) -> void;
		auto set_mouse_scrolled_callback(const std::function<void(float, float)>& mouse_scrolled_callback) -> void;
		auto set_mouse_button_pressed_callback(const std::function<void(int32_t)>& mouse_button_pressed_callback) -> void;
		auto set_mouse_button_released_callback(const std::function<void(int32_t)>& mouse_button_released_callback) -> void;
		auto set_window_close_callback(const std::function<void()>& window_close_callback) -> void;
		auto set_window_resize_callback(const std::function<void(int32_t, int32_t)>& window_resize_callback) -> void;
		auto set_window_framebuffer_resize_callback(const std::function<void(int32_t, int32_t)>& window_framebuffer_resize_callback) -> void;
		auto set_window_focus_callback(const std::function<void()>& window_focus_callback) -> void;
		auto set_window_lost_focus_callback(const std::function<void()>& window_lost_focus_callback) -> void;
		auto set_window_moved_callback(const std::function<void(int32_t, int32_t)>& window_moved_callback) -> void;

		auto graphics_api() const -> GraphicsApi;
		auto native_window() const -> GLFWwindow*;

	private:
		auto initialize_callbacks() -> void;

	private:
		Info m_info{};
		GraphicsApi m_graphics_api{ GraphicsApi::Vulkan };

		GLFWwindow* m_native_window{ nullptr };
	};
} // namespace HyperPlatform
