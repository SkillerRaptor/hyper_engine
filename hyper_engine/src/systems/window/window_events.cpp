/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "systems/window/window_events.hpp"

WindowMoveEvent::WindowMoveEvent(const uint32_t x, const uint32_t y)
    : m_x { x }
    , m_y { y }
{
}

WindowResizeEvent::WindowResizeEvent(const uint32_t width, const uint32_t height)
    : m_width { width }
    , m_height { height }
{
}
