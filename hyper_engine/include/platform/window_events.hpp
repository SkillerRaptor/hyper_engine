/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/types.hpp"

class WindowCloseEvent
{
public:
    WindowCloseEvent() = default;
};

class WindowMoveEvent
{
public:
    WindowMoveEvent(const u32 x, const u32 y)
        : m_x { x }
        , m_y { y }
    {
    }

    u32 x() const { return m_x; }
    u32 y() const { return m_y; }

private:
    u32 m_x { 0 };
    u32 m_y { 0 };
};

class WindowResizeEvent
{
public:
    WindowResizeEvent(const u32 width, const u32 height)
        : m_width { width }
        , m_height { height }
    {
    }

    u32 width() const { return m_width; }
    u32 height() const { return m_height; }

private:
    u32 m_width { 0 };
    u32 m_height { 0 };
};
