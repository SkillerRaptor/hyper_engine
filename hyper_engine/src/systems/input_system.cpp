/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "systems/input_system.hpp"

#include "core/prerequisites.hpp"

void InputSystem::initialize(WindowSystem &window_system)
{
    window_system.register_listener<MouseMoveEvent>(HE_BIND_FUNCTION(on_mouse_move));
    window_system.register_listener<MouseButtonPressEvent>(HE_BIND_FUNCTION(on_mouse_button_press));
    window_system.register_listener<MouseButtonReleaseEvent>(HE_BIND_FUNCTION(on_mouse_button_release));
    window_system.register_listener<KeyPressEvent>(HE_BIND_FUNCTION(on_key_press));
    window_system.register_listener<KeyReleaseEvent>(HE_BIND_FUNCTION(on_key_release));
}

bool InputSystem::is_key_pressed(const KeyCode key_code) const
{
    return m_keys.contains(key_code) ? m_keys.at(key_code) : false;
}

bool InputSystem::is_mouse_button_pressed(const MouseCode mouse_code) const
{
    return m_mouse_buttons.contains(mouse_code) ? m_mouse_buttons.at(mouse_code) : false;
}

void InputSystem::on_mouse_move(const MouseMoveEvent &event)
{
    const float x = event.x();
    const float y = event.y();

    m_mouse_position = { x, y };
}

void InputSystem::on_mouse_button_press(const MouseButtonPressEvent &event)
{
    const MouseCode mouse_code = event.mouse_code();
    m_mouse_buttons[mouse_code] = true;
}

void InputSystem::on_mouse_button_release(const MouseButtonReleaseEvent &event)
{
    const MouseCode mouse_code = event.mouse_code();
    m_mouse_buttons[mouse_code] = false;
}

void InputSystem::on_key_press(const KeyPressEvent &event)
{
    const KeyCode key_code = event.key_code();
    m_keys[key_code] = true;
}

void InputSystem::on_key_release(const KeyReleaseEvent &event)
{
    const KeyCode key_code = event.key_code();
    m_keys[key_code] = false;
}
