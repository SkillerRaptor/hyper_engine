/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "systems/window/mouse_events.hpp"

MouseMoveEvent::MouseMoveEvent(const float x, const float y)
    : m_x { x }
    , m_y { y }
{
}

MouseButtonPressEvent::MouseButtonPressEvent(const MouseCode mouse_code)
    : m_mouse_code { mouse_code }
{
}

MouseButtonReleaseEvent::MouseButtonReleaseEvent(const MouseCode mouse_code)
    : m_mouse_code { mouse_code }
{
}

MouseScrollEvent::MouseScrollEvent(const float delta_x, const float delta_y)
    : m_delta_x { delta_x }
    , m_delta_y { delta_y }
{
}
