/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_windowing/window_events.hpp"

namespace hyper_engine
{
    WindowMoveEvent::WindowMoveEvent(const uint32_t x, const uint32_t y)
        : m_x(x)
        , m_y(y)
    {
    }

    uint32_t WindowMoveEvent::x() const
    {
        return m_x;
    }

    uint32_t WindowMoveEvent::y() const
    {
        return m_y;
    }

    WindowResizeEvent::WindowResizeEvent(const uint32_t width, const uint32_t height)
        : m_width(width)
        , m_height(height)
    {
    }

    uint32_t WindowResizeEvent::width() const
    {
        return m_width;
    }

    uint32_t WindowResizeEvent::height() const
    {
        return m_height;
    }
} // namespace hyper_engine