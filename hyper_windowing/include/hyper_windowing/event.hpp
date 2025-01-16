/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_windowing/key_events.hpp"
#include "hyper_windowing/mouse_events.hpp"
#include "hyper_windowing/window_events.hpp"

namespace hyper_engine
{
    enum class EventType
    {
        KeyPress,
        KeyRelease,
        MouseButtonPress,
        MouseButtonRelease,
        MouseMove,
        MouseScroll,
        WindowClose,
        WindowMove,
        WindowResize,
    };

    struct Event
    {
        EventType type;

        union
        {
            KeyPressEvent key_press;
            KeyReleaseEvent key_release;
            MouseButtonPressEvent mouse_button_press;
            MouseButtonReleaseEvent mouse_button_release;
            MouseMoveEvent mouse_move;
            MouseScrollEvent mouse_scroll;
            WindowCloseEvent window_close;
            WindowMoveEvent window_move;
            WindowResizeEvent window_resize;
        };
    };
} // namespace hyper_engine