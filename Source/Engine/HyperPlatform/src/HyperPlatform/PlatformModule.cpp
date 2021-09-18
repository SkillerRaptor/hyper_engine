/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperPlatform/PlatformModule.hpp"

#include <HyperCore/Logger.hpp>

namespace HyperEngine
{
	auto PlatformModule::initialize() -> bool
	{
		/*
		m_window.set_key_pressed_callback(
			[this](int32_t key, bool repeat)
			{
				m_event_manager.invoke<HyperGame::KeyPressedEvent>(key, repeat);
			});

		m_window.set_key_released_callback(
			[this](int32_t key)
			{
				m_event_manager.invoke<HyperGame::KeyReleasedEvent>(key);
			});

		m_window.set_mouse_moved_callback(
			[this](float x, float y)
			{
				m_event_manager.invoke<HyperGame::MouseMovedEvent>(x, y);
			});

		m_window.set_mouse_scrolled_callback(
			[this](float x, float y)
			{
				m_event_manager.invoke<HyperGame::MouseScrolledEvent>(x, y);
			});

		m_window.set_mouse_button_pressed_callback(
			[this](int32_t button)
			{
				m_event_manager.invoke<HyperGame::MouseButtonPressedEvent>(static_cast<HyperGame::MouseButton>(button));
			});

		m_window.set_mouse_button_released_callback(
			[this](int32_t button)
			{
				m_event_manager.invoke<HyperGame::MouseButtonReleasedEvent>(static_cast<HyperGame::MouseButton>(button));
			});

		m_window.set_window_close_callback(
			[this]()
			{
				m_event_manager.invoke<HyperGame::WindowCloseEvent>();
			});

		m_window.set_window_resize_callback(
			[this](int32_t width, int32_t height)
			{
				m_event_manager.invoke<HyperGame::WindowResizeEvent>(width, height);
			});

		m_window.set_window_framebuffer_resize_callback(
			[this](int32_t width, int32_t height)
			{
				m_event_manager.invoke<HyperGame::WindowFramebufferResizeEvent>(width, height);
			});

		m_window.set_window_focus_callback(
			[this]()
			{
				m_event_manager.invoke<HyperGame::WindowLostFocusEvent>();
			});

		m_window.set_window_lost_focus_callback(
			[this]()
			{
				m_event_manager.invoke<HyperGame::WindowLostFocusEvent>();
			});

		m_window.set_window_moved_callback(
			[this](int32_t x, int32_t y)
			{
				m_event_manager.invoke<HyperGame::WindowMovedEvent>(x, y);
			});
		*/

		Logger::info("Successfully initialized Platform Module");

		return true;
	}

	auto PlatformModule::terminate() -> void
	{
		Logger::info("Successfully terminated Platform Module");
	}
	
	auto PlatformModule::name() const -> const char*
	{
		return "Platform";
	}
} // namespace HyperEngine
