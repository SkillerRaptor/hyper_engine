/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "systems/window/window_events.hpp"

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