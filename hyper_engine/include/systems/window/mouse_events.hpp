/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "systems/window/mouse_codes.hpp"

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

// FIXME: Change individual positions to vector
class MouseMoveEvent
{
public:
    MouseMoveEvent(const float x, const float y)
        : m_x { x }
        , m_y { y }
    {
    }

    float x() const { return m_x; }
    float y() const { return m_y; }

private:
    float m_x { 0.0 };
    float m_y { 0.0 };
};

// FIXME: Change individual deltas to vector
class MouseScrollEvent
{
public:
    MouseScrollEvent(const float delta_x, const float delta_y)
        : m_delta_x { delta_x }
        , m_delta_y { delta_y }
    {
    }

    float delta_x() const { return m_delta_x; }
    float delta_y() const { return m_delta_y; }

private:
    float m_delta_x { 0.0 };
    float m_delta_y { 0.0 };
};
