/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/types.hpp"
#include "platform/mouse_codes.hpp"

class MouseButtonPressEvent
{
public:
    explicit MouseButtonPressEvent(const MouseCode mouse_code)
        : m_mouse_code { mouse_code }
    {
    }

    MouseCode mouse_code() const { return m_mouse_code; }

private:
    MouseCode m_mouse_code { MouseCode::ButtonLeft };
};

class MouseButtonReleaseEvent
{
public:
    explicit MouseButtonReleaseEvent(const MouseCode mouse_code)
        : m_mouse_code { mouse_code }
    {
    }

    MouseCode mouse_code() const { return m_mouse_code; }

private:
    MouseCode m_mouse_code { MouseCode::ButtonLeft };
};

class MouseMoveEvent
{
public:
    MouseMoveEvent(const f32 x, const f32 y)
        : m_x { x }
        , m_y { y }
    {
    }

    f32 x() const { return m_x; }
    f32 y() const { return m_y; }

private:
    f32 m_x { 0.0 };
    f32 m_y { 0.0 };
};

class MouseScrollEvent
{
public:
    MouseScrollEvent(const f32 delta_x, const f32 delta_y)
        : m_delta_x { delta_x }
        , m_delta_y { delta_y }
    {
    }

    f32 delta_x() const { return m_delta_x; }
    f32 delta_y() const { return m_delta_y; }

private:
    f32 m_delta_x { 0.0 };
    f32 m_delta_y { 0.0 };
};
