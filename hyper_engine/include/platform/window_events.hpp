/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/prerequisites.hpp"
#include "core/types.hpp"

namespace he
{
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

        HE_ALWAYS_INLINE u32 x() const { return m_x; }
        HE_ALWAYS_INLINE u32 y() const { return m_y; }

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

        HE_ALWAYS_INLINE u32 width() const { return m_width; }
        HE_ALWAYS_INLINE u32 height() const { return m_height; }

    private:
        u32 m_width { 0 };
        u32 m_height { 0 };
    };
} // namespace he
