/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_platform/window_events.hpp"

namespace hyper_platform
{
    WindowResizeEvent::WindowResizeEvent(const uint32_t width, const uint32_t height)
        : m_width(width)
        , m_height((height))
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

    WindowFramebufferResizeEvent::WindowFramebufferResizeEvent(const uint32_t width, const uint32_t height)
        : m_width(width)
        , m_height((height))
    {
    }

    uint32_t WindowFramebufferResizeEvent::width() const
    {
        return m_width;
    }

    uint32_t WindowFramebufferResizeEvent::height() const
    {
        return m_height;
    }
} // namespace hyper_platform
