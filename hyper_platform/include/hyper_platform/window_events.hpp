/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace hyper_platform
{
    class WindowCloseEvent
    {
    public:
        WindowCloseEvent() = default;
    };

    class WindowResizeEvent
    {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height);

        [[nodiscard]] uint32_t width() const;
        [[nodiscard]] uint32_t height() const;

    private:
        uint32_t m_width;
        uint32_t m_height;
    };

    class WindowFramebufferResizeEvent
    {
    public:
        WindowFramebufferResizeEvent(uint32_t width, uint32_t height);

        uint32_t width() const;
        uint32_t height() const;

    private:
        uint32_t m_width;
        uint32_t m_height;
    };
} // namespace hyper_platform
