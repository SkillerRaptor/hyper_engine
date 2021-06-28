/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Assertion.hpp>
#include <HyperCore/Events/Event.hpp>

namespace HyperCore
{
	CEvent::CEvent(SKeyPressedEvent key_pressed_event)
		: m_type(CEvent::EType::KeyPressed)
	{
		m_value.as_key_pressed_event = key_pressed_event;
	}

	CEvent::CEvent(SKeyReleasedEvent key_released_event)
		: m_type(CEvent::EType::KeyReleased)
	{
		m_value.as_key_released_event = key_released_event;
	}

	CEvent::CEvent(SKeyTypedEvent key_typed_event)
		: m_type(CEvent::EType::KeyTyped)
	{
		m_value.as_key_typed_event = key_typed_event;
	}

	CEvent::CEvent(SMouseMovedEvent mouse_moved_event)
		: m_type(CEvent::EType::MouseMoved)
	{
		m_value.as_mouse_moved_event = mouse_moved_event;
	}

	CEvent::CEvent(SMouseScrolledEvent mouse_scrolled_event)
		: m_type(CEvent::EType::MouseScrolled)
	{
		m_value.as_mouse_scrolled_event = mouse_scrolled_event;
	}

	CEvent::CEvent(SMouseButtonPressedEvent mouse_button_pressed_event)
		: m_type(CEvent::EType::MouseButtonPressed)
	{
		m_value.as_mouse_button_pressed_event = mouse_button_pressed_event;
	}

	CEvent::CEvent(SMouseButtonReleasedEvent mouse_button_released_event)
		: m_type(CEvent::EType::MouseButtonReleased)
	{
		m_value.as_mouse_button_released_event = mouse_button_released_event;
	}

	CEvent::CEvent(SWindowCloseEvent window_close_event)
		: m_type(CEvent::EType::WindowClose)
	{
		m_value.as_window_close_event = window_close_event;
	}

	CEvent::CEvent(SWindowResizeEvent window_resize_event)
		: m_type(CEvent::EType::WindowResize)
	{
		m_value.as_window_resize_event = window_resize_event;
	}

	CEvent::CEvent(SWindowFocusEvent window_focus_event)
		: m_type(CEvent::EType::WindowFocus)
	{
		m_value.as_window_focus_event = window_focus_event;
	}

	CEvent::CEvent(SWindowLostFocusEvent window_lost_focus_event)
		: m_type(CEvent::EType::WindowLostFocus)
	{
		m_value.as_window_lost_focus_event = window_lost_focus_event;
	}

	CEvent::CEvent(SWindowMovedEvent window_moved_event)
		: m_type(CEvent::EType::WindowMoved)
	{
		m_value.as_window_moved_event = window_moved_event;
	}

	CEvent::EType CEvent::type() const
	{
		return m_type;
	}

	bool CEvent::is_key_pressed_event() const
	{
		return m_type == EType::KeyPressed;
	}

	bool CEvent::is_key_released_event() const
	{
		return m_type == EType::KeyReleased;
	}

	bool CEvent::is_key_typed_event() const
	{
		return m_type == EType::KeyTyped;
	}

	bool CEvent::is_mouse_moved_event() const
	{
		return m_type == EType::MouseMoved;
	}

	bool CEvent::is_mouse_scrolled_event() const
	{
		return m_type == EType::MouseScrolled;
	}

	bool CEvent::is_mouse_button_pressed_event() const
	{
		return m_type == EType::MouseButtonPressed;
	}

	bool CEvent::is_mouse_button_released_event() const
	{
		return m_type == EType::MouseButtonReleased;
	}

	bool CEvent::is_window_close_event() const
	{
		return m_type == EType::WindowClose;
	}

	bool CEvent::is_window_resize_event() const
	{
		return m_type == EType::WindowResize;
	}

	bool CEvent::is_window_focus_event() const
	{
		return m_type == EType::WindowFocus;
	}

	bool CEvent::is_window_lost_focus_event() const
	{
		return m_type == EType::WindowLostFocus;
	}

	bool CEvent::is_window_moved_event() const
	{
		return m_type == EType::WindowMoved;
	}

	SKeyPressedEvent CEvent::as_key_pressed_event() const
	{
		HYPERENGINE_ASSERT(is_key_pressed_event());
		return m_value.as_key_pressed_event;
	}

	SKeyReleasedEvent CEvent::as_key_released_event() const
	{
		HYPERENGINE_ASSERT(is_key_released_event());
		return m_value.as_key_released_event;
	}

	SKeyTypedEvent CEvent::as_key_typed_event() const
	{
		HYPERENGINE_ASSERT(is_key_typed_event());
		return m_value.as_key_typed_event;
	}

	SMouseMovedEvent CEvent::as_mouse_moved_event() const
	{
		HYPERENGINE_ASSERT(is_mouse_moved_event());
		return m_value.as_mouse_moved_event;
	}

	SMouseScrolledEvent CEvent::as_mouse_scrolled_event() const
	{
		HYPERENGINE_ASSERT(is_mouse_scrolled_event());
		return m_value.as_mouse_scrolled_event;
	}

	SMouseButtonPressedEvent CEvent::as_mouse_button_pressed_event() const
	{
		HYPERENGINE_ASSERT(is_mouse_button_pressed_event());
		return m_value.as_mouse_button_pressed_event;
	}

	SMouseButtonReleasedEvent CEvent::as_mouse_button_released_event() const
	{
		HYPERENGINE_ASSERT(is_mouse_button_released_event());
		return m_value.as_mouse_button_released_event;
	}

	SWindowCloseEvent CEvent::as_window_close_event() const
	{
		HYPERENGINE_ASSERT(is_window_close_event());
		return m_value.as_window_close_event;
	}

	SWindowResizeEvent CEvent::as_window_resize_event() const
	{
		HYPERENGINE_ASSERT(is_window_resize_event());
		return m_value.as_window_resize_event;
	}

	SWindowFocusEvent CEvent::as_window_focus_event() const
	{
		HYPERENGINE_ASSERT(is_window_focus_event());
		return m_value.as_window_focus_event;
	}

	SWindowLostFocusEvent CEvent::as_window_lost_focus_event() const
	{
		HYPERENGINE_ASSERT(is_window_lost_focus_event());
		return m_value.as_window_lost_focus_event;
	}

	SWindowMovedEvent CEvent::as_window_moved_event() const
	{
		HYPERENGINE_ASSERT(is_window_moved_event());
		return m_value.as_window_moved_event;
	}
} // namespace HyperCore
