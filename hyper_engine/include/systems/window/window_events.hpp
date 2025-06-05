/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

class WindowCloseEvent
{
public:
    WindowCloseEvent() = default;
};

// FIXME: Change individual positions to vector
class WindowMoveEvent
{
public:
    WindowMoveEvent(uint32_t x, uint32_t y);

    uint32_t x() const { return m_x; }
    uint32_t y() const { return m_y; }

private:
    uint32_t m_x { 0 };
    uint32_t m_y { 0 };
};

// FIXME: Change individual sizes to vector
class WindowResizeEvent
{
public:
    WindowResizeEvent(uint32_t width, uint32_t height);

    uint32_t width() const { return m_width; }
    uint32_t height() const { return m_height; }

private:
    uint32_t m_width { 0 };
    uint32_t m_height { 0 };
};
