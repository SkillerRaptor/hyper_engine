/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_platform/mouse_events.hpp"

namespace hyper_platform
{
    MouseMovedEvent::MouseMovedEvent(float x, float y)
        : m_x(x)
        , m_y(y)
    {
    }

    float MouseMovedEvent::x() const
    {
        return m_x;
    }

    float MouseMovedEvent::y() const
    {
        return m_y;
    }

    MouseScrolledEvent::MouseScrolledEvent(float delta_x, float delta_y)
        : m_delta_x(delta_x)
        , m_delta_y(delta_y)
    {
    }

    float MouseScrolledEvent::delta_x() const
    {
        return m_delta_x;
    }

    float MouseScrolledEvent::delta_y() const
    {
        return m_delta_y;
    }

    MouseButtonPressedEvent::MouseButtonPressedEvent(MouseCode mouse_code)
        : m_mouse_code(mouse_code)
    {
    }

    MouseCode MouseButtonPressedEvent::mouse_code() const
    {
        return m_mouse_code;
    }

    MouseButtonReleasedEvent::MouseButtonReleasedEvent(MouseCode mouse_code)
        : m_mouse_code(mouse_code)
    {
    }

    MouseCode MouseButtonReleasedEvent::mouse_code() const
    {
        return m_mouse_code;
    }
} // namespace hyper_platform
