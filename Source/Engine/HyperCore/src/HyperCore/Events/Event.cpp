/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Events/Event.hpp"

namespace HyperCore
{
	Event::Event(KeyPressedEvent key_pressed_event)
		: type(Event::Type::KeyPressed)
	{
		value.as_key_pressed_event = key_pressed_event;
	}

	Event::Event(KeyReleasedEvent key_released_event)
		: type(Event::Type::KeyReleased)
	{
		value.as_key_released_event = key_released_event;
	}

	Event::Event(MouseMovedEvent mouse_moved_event)
		: type(Event::Type::MouseMoved)
	{
		value.as_mouse_moved_event = mouse_moved_event;
	}

	Event::Event(MouseScrolledEvent mouse_scrolled_event)
		: type(Event::Type::MouseScrolled)
	{
		value.as_mouse_scrolled_event = mouse_scrolled_event;
	}

	Event::Event(MouseButtonPressedEvent mouse_button_pressed_event)
		: type(Event::Type::MouseButtonPressed)
	{
		value.as_mouse_button_pressed_event = mouse_button_pressed_event;
	}

	Event::Event(MouseButtonReleasedEvent mouse_button_released_event)
		: type(Event::Type::MouseButtonReleased)
	{
		value.as_mouse_button_released_event = mouse_button_released_event;
	}

	Event::Event(WindowCloseEvent window_close_event)
		: type(Event::Type::WindowClose)
	{
		value.as_window_close_event = window_close_event;
	}

	Event::Event(WindowResizeEvent window_resize_event)
		: type(Event::Type::WindowResize)
	{
		value.as_window_resize_event = window_resize_event;
	}

	Event::Event(WindowFramebufferResizeEvent window_framebuffer_resize_event)
	: type(Event::Type::WindowFramebufferResize)
	{
		value.as_window_framebuffer_resize_event = window_framebuffer_resize_event;
	}

	Event::Event(WindowFocusEvent window_focus_event)
		: type(Event::Type::WindowFocus)
	{
		value.as_window_focus_event = window_focus_event;
	}

	Event::Event(WindowLostFocusEvent window_lost_focus_event)
		: type(Event::Type::WindowLostFocus)
	{
		value.as_window_lost_focus_event = window_lost_focus_event;
	}

	Event::Event(WindowMovedEvent window_moved_event)
		: type(Event::Type::WindowMoved)
	{
		value.as_window_moved_event = window_moved_event;
	}
} // namespace HyperCore
