/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/surface.hpp"

namespace hyper_engine
{
    void Surface::resize(const uint32_t width, const uint32_t height)
    {
        if (m_width == width || m_height == height)
        {
            return;
        }

        m_resized = true;
        m_width = width;
        m_height = height;
    }

    bool Surface::resized() const
    {
        return m_resized;
    }

    uint32_t Surface::width() const
    {
        return m_width;
    }

    uint32_t Surface::height() const
    {
        return m_height;
    }
} // namespace hyper_engine