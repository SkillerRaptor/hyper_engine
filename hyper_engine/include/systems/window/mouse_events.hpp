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
    explicit MouseButtonPressEvent(MouseCode mouse_code);

    MouseCode mouse_code() const;

private:
    MouseCode m_mouse_code = MouseCode::ButtonLeft;
};

class MouseButtonReleaseEvent
{
public:
    explicit MouseButtonReleaseEvent(MouseCode mouse_code);

    MouseCode mouse_code() const;

private:
    MouseCode m_mouse_code = MouseCode::ButtonLeft;
};

// FIXME: Change individual positions to vector
class MouseMoveEvent
{
public:
    MouseMoveEvent(float x, float y);

    float x() const;
    float y() const;

private:
    float m_x = 0.0;
    float m_y = 0.0;
};

// FIXME: Change individual deltas to vector
class MouseScrollEvent
{
public:
    MouseScrollEvent(float delta_x, float delta_y);

    float delta_x() const;
    float delta_y() const;

private:
    float m_delta_x = 0.0;
    float m_delta_y = 0.0;
};