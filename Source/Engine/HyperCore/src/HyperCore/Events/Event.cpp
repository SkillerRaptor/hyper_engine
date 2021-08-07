/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Events/Event.hpp"

#include "HyperCore/Assertion.hpp"

namespace HyperCore
{
	Event::Event(KeyPressedEvent key_pressed_event)
		: m_type(Event::Type::KeyPressed)
	{
		m_value.as_key_pressed_event = key_pressed_event;
	}

	Event::Event(KeyReleasedEvent key_released_event)
		: m_type(Event::Type::KeyReleased)
	{
		m_value.as_key_released_event = key_released_event;
	}

	Event::Event(KeyTypedEvent key_typed_event)
		: m_type(Event::Type::KeyTyped)
	{
		m_value.as_key_typed_event = key_typed_event;
	}

	Event::Event(MouseMovedEvent mouse_moved_event)
		: m_type(Event::Type::MouseMoved)
	{
		m_value.as_mouse_moved_event = mouse_moved_event;
	}

	Event::Event(MouseScrolledEvent mouse_scrolled_event)
		: m_type(Event::Type::MouseScrolled)
	{
		m_value.as_mouse_scrolled_event = mouse_scrolled_event;
	}

	Event::Event(MouseButtonPressedEvent mouse_button_pressed_event)
		: m_type(Event::Type::MouseButtonPressed)
	{
		m_value.as_mouse_button_pressed_event = mouse_button_pressed_event;
	}

	Event::Event(MouseButtonReleasedEvent mouse_button_released_event)
		: m_type(Event::Type::MouseButtonReleased)
	{
		m_value.as_mouse_button_released_event = mouse_button_released_event;
	}

	Event::Event(WindowCloseEvent window_close_event)
		: m_type(Event::Type::WindowClose)
	{
		m_value.as_window_close_event = window_close_event;
	}

	Event::Event(WindowResizeEvent window_resize_event)
		: m_type(Event::Type::WindowResize)
	{
		m_value.as_window_resize_event = window_resize_event;
	}

	Event::Event(WindowFocusEvent window_focus_event)
		: m_type(Event::Type::WindowFocus)
	{
		m_value.as_window_focus_event = window_focus_event;
	}

	Event::Event(WindowLostFocusEvent window_lost_focus_event)
		: m_type(Event::Type::WindowLostFocus)
	{
		m_value.as_window_lost_focus_event = window_lost_focus_event;
	}

	Event::Event(WindowMovedEvent window_moved_event)
		: m_type(Event::Type::WindowMoved)
	{
		m_value.as_window_moved_event = window_moved_event;
	}

	Event::Type Event::type() const noexcept
	{
		return m_type;
	}

	bool Event::is_key_pressed_event() const noexcept
	{
		return m_type == Type::KeyPressed;
	}

	bool Event::is_key_released_event() const noexcept
	{
		return m_type == Type::KeyReleased;
	}

	bool Event::is_key_typed_event() const noexcept
	{
		return m_type == Type::KeyTyped;
	}

	bool Event::is_mouse_moved_event() const noexcept
	{
		return m_type == Type::MouseMoved;
	}

	bool Event::is_mouse_scrolled_event() const noexcept
	{
		return m_type == Type::MouseScrolled;
	}

	bool Event::is_mouse_button_pressed_event() const noexcept
	{
		return m_type == Type::MouseButtonPressed;
	}

	bool Event::is_mouse_button_released_event() const noexcept
	{
		return m_type == Type::MouseButtonReleased;
	}

	bool Event::is_window_close_event() const noexcept
	{
		return m_type == Type::WindowClose;
	}

	bool Event::is_window_resize_event() const noexcept
	{
		return m_type == Type::WindowResize;
	}

	bool Event::is_window_focus_event() const noexcept
	{
		return m_type == Type::WindowFocus;
	}

	bool Event::is_window_lost_focus_event() const noexcept
	{
		return m_type == Type::WindowLostFocus;
	}

	bool Event::is_window_moved_event() const noexcept
	{
		return m_type == Type::WindowMoved;
	}

	KeyPressedEvent Event::as_key_pressed_event() const
	{
		HYPERENGINE_ASSERT(is_key_pressed_event());
		return m_value.as_key_pressed_event;
	}

	KeyReleasedEvent Event::as_key_released_event() const
	{
		HYPERENGINE_ASSERT(is_key_released_event());
		return m_value.as_key_released_event;
	}

	KeyTypedEvent Event::as_key_typed_event() const
	{
		HYPERENGINE_ASSERT(is_key_typed_event());
		return m_value.as_key_typed_event;
	}

	MouseMovedEvent Event::as_mouse_moved_event() const
	{
		HYPERENGINE_ASSERT(is_mouse_moved_event());
		return m_value.as_mouse_moved_event;
	}

	MouseScrolledEvent Event::as_mouse_scrolled_event() const
	{
		HYPERENGINE_ASSERT(is_mouse_scrolled_event());
		return m_value.as_mouse_scrolled_event;
	}

	MouseButtonPressedEvent Event::as_mouse_button_pressed_event() const
	{
		HYPERENGINE_ASSERT(is_mouse_button_pressed_event());
		return m_value.as_mouse_button_pressed_event;
	}

	MouseButtonReleasedEvent Event::as_mouse_button_released_event() const
	{
		HYPERENGINE_ASSERT(is_mouse_button_released_event());
		return m_value.as_mouse_button_released_event;
	}

	WindowCloseEvent Event::as_window_close_event() const
	{
		HYPERENGINE_ASSERT(is_window_close_event());
		return m_value.as_window_close_event;
	}

	WindowResizeEvent Event::as_window_resize_event() const
	{
		HYPERENGINE_ASSERT(is_window_resize_event());
		return m_value.as_window_resize_event;
	}

	WindowFocusEvent Event::as_window_focus_event() const
	{
		HYPERENGINE_ASSERT(is_window_focus_event());
		return m_value.as_window_focus_event;
	}

	WindowLostFocusEvent Event::as_window_lost_focus_event() const
	{
		HYPERENGINE_ASSERT(is_window_lost_focus_event());
		return m_value.as_window_lost_focus_event;
	}

	WindowMovedEvent Event::as_window_moved_event() const
	{
		HYPERENGINE_ASSERT(is_window_moved_event());
		return m_value.as_window_moved_event;
	}
} // namespace HyperCore
