/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Events/EventManager.hpp"

namespace HyperCore
{
	EventManager::EventManager()
	{
		register_event<KeyPressedEvent>();
		register_event<KeyReleasedEvent>();
		register_event<KeyTypedEvent>();
		register_event<MouseMovedEvent>();
		register_event<MouseScrolledEvent>();
		register_event<MouseButtonPressedEvent>();
		register_event<MouseButtonReleasedEvent>();
		register_event<WindowCloseEvent>();
		register_event<WindowResizeEvent>();
		register_event<WindowFocusEvent>();
		register_event<WindowLostFocusEvent>();
		register_event<WindowMovedEvent>();
	}
	
	EventManager::~EventManager()
	{
		for (auto& [id, event_wrapper_base] : m_event_wrappers)
		{
			deallocate(event_wrapper_base);
		}
	}
	
	void EventManager::process_next_event()
	{
		if (m_event_bus.empty())
		{
			return;
		}

		const Event& event = m_event_bus.front();
		switch (event.type())
		{
		case Event::Type::KeyPressed:
			invoke_event<KeyPressedEvent>(event.as_key_pressed_event());
			break;
		case Event::Type::KeyReleased:
			invoke_event<KeyReleasedEvent>(event.as_key_released_event());
			break;
		case Event::Type::KeyTyped:
			invoke_event<KeyTypedEvent>(event.as_key_typed_event());
			break;
		case Event::Type::MouseMoved:
			invoke_event<MouseMovedEvent>(event.as_mouse_moved_event());
			break;
		case Event::Type::MouseScrolled:
			invoke_event<MouseScrolledEvent>(event.as_mouse_scrolled_event());
			break;
		case Event::Type::MouseButtonPressed:
			invoke_event<MouseButtonPressedEvent>(event.as_mouse_button_pressed_event());
			break;
		case Event::Type::MouseButtonReleased:
			invoke_event<MouseButtonReleasedEvent>(event.as_mouse_button_released_event());
			break;
		case Event::Type::WindowClose:
			invoke_event<WindowCloseEvent>(event.as_window_close_event());
			break;
		case Event::Type::WindowResize:
			invoke_event<WindowResizeEvent>(event.as_window_resize_event());
			break;
		case Event::Type::WindowFocus:
			invoke_event<WindowFocusEvent>(event.as_window_focus_event());
			break;
		case Event::Type::WindowLostFocus:
			invoke_event<WindowLostFocusEvent>(event.as_window_lost_focus_event());
			break;
		case Event::Type::WindowMoved:
			invoke_event<WindowMovedEvent>(event.as_window_moved_event());
			break;
		default:
			break;
		}

		m_event_bus.pop();
	}
} // namespace HyperCore
