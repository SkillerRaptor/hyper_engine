/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Compilers.hpp"
#include "HyperCore/Events/KeyEvents.hpp"
#include "HyperCore/Events/MouseEvents.hpp"
#include "HyperCore/Events/WindowEvents.hpp"

#include <cstdint>

HYPERENGINE_COMPILER_PUSH_WARNING
HYPERENGINE_COMPILER_PUSH_ANONYMOUS_STRUCT

namespace HyperCore
{
	/*
	 * TODO: Rework Event System
	 */
	
	class Event
	{
	public:
		enum class Type : uint8_t
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
		explicit Event(KeyPressedEvent key_pressed_event);
		explicit Event(KeyReleasedEvent key_released_event);
		explicit Event(KeyTypedEvent key_typed_event);
		explicit Event(MouseMovedEvent mouse_moved_event);
		explicit Event(MouseScrolledEvent mouse_scrolled_event);
		explicit Event(MouseButtonPressedEvent mouse_button_pressed_event);
		explicit Event(MouseButtonReleasedEvent mouse_button_released_event);
		explicit Event(WindowCloseEvent window_close_event);
		explicit Event(WindowResizeEvent window_resize_event);
		explicit Event(WindowFocusEvent window_focus_event);
		explicit Event(WindowLostFocusEvent window_lost_focus_event);
		explicit Event(WindowMovedEvent window_moved_event);

		Type type() const;

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

		KeyPressedEvent as_key_pressed_event() const;
		KeyReleasedEvent as_key_released_event() const;
		KeyTypedEvent as_key_typed_event() const;
		MouseMovedEvent as_mouse_moved_event() const;
		MouseScrolledEvent as_mouse_scrolled_event() const;
		MouseButtonPressedEvent as_mouse_button_pressed_event() const;
		MouseButtonReleasedEvent as_mouse_button_released_event() const;
		WindowCloseEvent as_window_close_event() const;
		WindowResizeEvent as_window_resize_event() const;
		WindowFocusEvent as_window_focus_event() const;
		WindowLostFocusEvent as_window_lost_focus_event() const;
		WindowMovedEvent as_window_moved_event() const;

	private:
		Type m_type{ Type::Undefined };

		union
		{
			KeyPressedEvent as_key_pressed_event;
			KeyReleasedEvent as_key_released_event;
			KeyTypedEvent as_key_typed_event;
			MouseMovedEvent as_mouse_moved_event;
			MouseScrolledEvent as_mouse_scrolled_event;
			MouseButtonPressedEvent as_mouse_button_pressed_event;
			MouseButtonReleasedEvent as_mouse_button_released_event;
			WindowCloseEvent as_window_close_event;
			WindowResizeEvent as_window_resize_event;
			WindowFocusEvent as_window_focus_event;
			WindowLostFocusEvent as_window_lost_focus_event;
			WindowMovedEvent as_window_moved_event;
		} m_value{};
	};
} // namespace HyperCore

HYPERENGINE_COMPILER_POP_WARNING
