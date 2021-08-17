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
		register_event<MouseMovedEvent>();
		register_event<MouseScrolledEvent>();
		register_event<MouseButtonPressedEvent>();
		register_event<MouseButtonReleasedEvent>();
		register_event<WindowCloseEvent>();
		register_event<WindowResizeEvent>();
		register_event<WindowFramebufferResizeEvent>();
		register_event<WindowFocusEvent>();
		register_event<WindowLostFocusEvent>();
		register_event<WindowMovedEvent>();
	}

	EventManager::~EventManager()
	{
		for (auto& event_wrapper : m_event_wrappers)
		{
			delete event_wrapper.second;
		}
	}

	void EventManager::process_next_event()
	{
		if (m_event_bus.empty())
		{
			return;
		}

		const Event& event = m_event_bus.front();
		switch (event.type)
		{
		case Event::Type::key_pressed:
			invoke_event<KeyPressedEvent>(event.value.as_key_pressed_event);
			break;
		case Event::Type::key_released:
			invoke_event<KeyReleasedEvent>(event.value.as_key_released_event);
			break;
		case Event::Type::mouse_moved:
			invoke_event<MouseMovedEvent>(event.value.as_mouse_moved_event);
			break;
		case Event::Type::mouse_scrolled:
			invoke_event<MouseScrolledEvent>(event.value.as_mouse_scrolled_event);
			break;
		case Event::Type::mouse_button_pressed:
			invoke_event<MouseButtonPressedEvent>(event.value.as_mouse_button_pressed_event);
			break;
		case Event::Type::mouse_button_released:
			invoke_event<MouseButtonReleasedEvent>(event.value.as_mouse_button_released_event);
			break;
		case Event::Type::window_close:
			invoke_event<WindowCloseEvent>(event.value.as_window_close_event);
			break;
		case Event::Type::window_resize:
			invoke_event<WindowResizeEvent>(event.value.as_window_resize_event);
			break;
		case Event::Type::window_framebuffer_resize:
			invoke_event<WindowFramebufferResizeEvent>(event.value.as_window_framebuffer_resize_event);
			break;
		case Event::Type::window_focus:
			invoke_event<WindowFocusEvent>(event.value.as_window_focus_event);
			break;
		case Event::Type::window_lost_focus:
			invoke_event<WindowLostFocusEvent>(event.value.as_window_lost_focus_event);
			break;
		case Event::Type::window_moved:
			invoke_event<WindowMovedEvent>(event.value.as_window_moved_event);
			break;
		default:
			break;
		}

		m_event_bus.pop();
	}
} // namespace HyperCore
