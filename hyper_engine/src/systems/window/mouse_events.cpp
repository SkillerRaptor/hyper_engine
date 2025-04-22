/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "systems/window/mouse_events.hpp"

MouseMoveEvent::MouseMoveEvent(const float x, const float y)
    : m_x(x)
    , m_y(y)
{
}

float MouseMoveEvent::x() const
{
    return m_x;
}

float MouseMoveEvent::y() const
{
    return m_y;
}

MouseButtonPressEvent::MouseButtonPressEvent(const MouseCode mouse_code)
    : m_mouse_code(mouse_code)
{
}

MouseCode MouseButtonPressEvent::mouse_code() const
{
    return m_mouse_code;
}

MouseButtonReleaseEvent::MouseButtonReleaseEvent(const MouseCode mouse_code)
    : m_mouse_code(mouse_code)
{
}

MouseCode MouseButtonReleaseEvent::mouse_code() const
{
    return m_mouse_code;
}

MouseScrollEvent::MouseScrollEvent(const float delta_x, const float delta_y)
    : m_delta_x(delta_x)
    , m_delta_y(delta_y)
{
}

float MouseScrollEvent::delta_x() const
{
    return m_delta_x;
}

float MouseScrollEvent::delta_y() const
{
    return m_delta_y;
}