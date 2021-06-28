/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Events/KeyEvents.hpp>
#include <HyperCore/Events/MouseEvents.hpp>
#include <HyperCore/Events/WindowEvents.hpp>

namespace HyperCore
{
	class CEvent
	{
	public:
		enum class EType
		{
			Undefined,

			KeyPressed,
			KeyReleased,
			KeyTyped,

			MouseMoved,
			MouseScrolled,
			MouseButtonPressed,
			MouseButtonReleased,

			WindowClose,
			WindowResize,
			WindowFocus,
			WindowLostFocus,
			WindowMoved
		};

	public:
		explicit CEvent(SKeyPressedEvent key_pressed_event);
		explicit CEvent(SKeyReleasedEvent key_released_event);
		explicit CEvent(SKeyTypedEvent key_typed_event);
		explicit CEvent(SMouseMovedEvent mouse_moved_event);
		explicit CEvent(SMouseScrolledEvent mouse_scrolled_event);
		explicit CEvent(SMouseButtonPressedEvent mouse_button_pressed_event);
		explicit CEvent(SMouseButtonReleasedEvent mouse_button_released_event);
		explicit CEvent(SWindowCloseEvent window_close_event);
		explicit CEvent(SWindowResizeEvent window_resize_event);
		explicit CEvent(SWindowFocusEvent window_focus_event);
		explicit CEvent(SWindowLostFocusEvent window_lost_focus_event);
		explicit CEvent(SWindowMovedEvent window_moved_event);

		EType type() const;

		bool is_key_pressed_event() const;
		bool is_key_released_event() const;
		bool is_key_typed_event() const;
		bool is_mouse_moved_event() const;
		bool is_mouse_scrolled_event() const;
		bool is_mouse_button_pressed_event() const;
		bool is_mouse_button_released_event() const;
		bool is_window_close_event() const;
		bool is_window_resize_event() const;
		bool is_window_focus_event() const;
		bool is_window_lost_focus_event() const;
		bool is_window_moved_event() const;

		SKeyPressedEvent as_key_pressed_event() const;
		SKeyReleasedEvent as_key_released_event() const;
		SKeyTypedEvent as_key_typed_event() const;
		SMouseMovedEvent as_mouse_moved_event() const;
		SMouseScrolledEvent as_mouse_scrolled_event() const;
		SMouseButtonPressedEvent as_mouse_button_pressed_event() const;
		SMouseButtonReleasedEvent as_mouse_button_released_event() const;
		SWindowCloseEvent as_window_close_event() const;
		SWindowResizeEvent as_window_resize_event() const;
		SWindowFocusEvent as_window_focus_event() const;
		SWindowLostFocusEvent as_window_lost_focus_event() const;
		SWindowMovedEvent as_window_moved_event() const;

	private:
		EType m_type{ EType::Undefined };

		union
		{
			SKeyPressedEvent as_key_pressed_event;
			SKeyReleasedEvent as_key_released_event;
			SKeyTypedEvent as_key_typed_event;
			SMouseMovedEvent as_mouse_moved_event;
			SMouseScrolledEvent as_mouse_scrolled_event;
			SMouseButtonPressedEvent as_mouse_button_pressed_event;
			SMouseButtonReleasedEvent as_mouse_button_released_event;
			SWindowCloseEvent as_window_close_event;
			SWindowResizeEvent as_window_resize_event;
			SWindowFocusEvent as_window_focus_event;
			SWindowLostFocusEvent as_window_lost_focus_event;
			SWindowMovedEvent as_window_moved_event;
		} m_value{};
	};
} // namespace HyperCore
