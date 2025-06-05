/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "systems/window/key_events.hpp"
#include "systems/window/mouse_events.hpp"
#include "systems/window_system.hpp"

class InputSystem
{
public:
    void initialize(WindowSystem &window_system);

    // Keys
    bool is_key_pressed(KeyCode key_code) const;

    // Mouse
    bool is_mouse_button_pressed(MouseCode mouse_code) const;
    glm::vec2 mouse_position() const { return m_mouse_position; }

private:
    void on_mouse_move(const MouseMoveEvent &event);
    void on_mouse_button_press(const MouseButtonPressEvent &event);
    void on_mouse_button_release(const MouseButtonReleaseEvent &event);
    void on_key_press(const KeyPressEvent &event);
    void on_key_release(const KeyReleaseEvent &event);

private:
    std::unordered_map<KeyCode, bool> m_keys {};
    std::unordered_map<MouseCode, bool> m_mouse_buttons {};
    glm::vec2 m_mouse_position { 0.0f, 0.0f };
};
