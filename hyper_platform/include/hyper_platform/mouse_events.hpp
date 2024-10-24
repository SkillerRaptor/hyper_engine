/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_platform/mouse_codes.hpp"

namespace hyper_platform
{
    class MouseMovedEvent
    {
    public:
        MouseMovedEvent(float x, float y);

        [[nodiscard]] float x() const;
        [[nodiscard]] float y() const;

    private:
        float m_x;
        float m_y;
    };

    class MouseScrolledEvent
    {
    public:
        MouseScrolledEvent(float delta_x, float delta_y);

        [[nodiscard]] float delta_x() const;
        [[nodiscard]] float delta_y() const;

    private:
        float m_delta_x;
        float m_delta_y;
    };

    class MouseButtonPressedEvent
    {
    public:
        explicit MouseButtonPressedEvent(MouseCode mouse_code);

        [[nodiscard]] MouseCode mouse_code() const;

    private:
        MouseCode m_mouse_code;
    };

    class MouseButtonReleasedEvent
    {
    public:
        explicit MouseButtonReleasedEvent(MouseCode mouse_code);

        [[nodiscard]] MouseCode mouse_code() const;

    private:
        MouseCode m_mouse_code;
    };
} // namespace hyper_platform
