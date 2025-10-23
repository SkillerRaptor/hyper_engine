/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "platform/input.hpp"

#include "core/logger.hpp"
#include "platform/event_loop.hpp"

namespace he
{
    Input::Input(EventLoop &event_loop)
    {
        event_loop.add_listener<MouseMoveEvent>(HE_BIND_FUNCTION(on_mouse_move));
        event_loop.add_listener<MouseButtonPressEvent>(HE_BIND_FUNCTION(on_mouse_button_press));
        event_loop.add_listener<MouseButtonReleaseEvent>(HE_BIND_FUNCTION(on_mouse_button_release));
        event_loop.add_listener<KeyPressEvent>(HE_BIND_FUNCTION(on_key_press));
        event_loop.add_listener<KeyReleaseEvent>(HE_BIND_FUNCTION(on_key_release));
    }

    bool Input::is_key_pressed(const KeyCode key_code) const
    {
        const auto it = m_keys.find(key_code);
        return it != m_keys.end() && it->second;
    }

    bool Input::is_mouse_button_pressed(const MouseCode mouse_code) const
    {
        const auto it = m_mouse_buttons.find(mouse_code);
        return it != m_mouse_buttons.end() && it->second;
    }

    void Input::on_mouse_move(const MouseMoveEvent &event)
    {
        const f32 x = event.x();
        const f32 y = event.y();
        m_mouse_position = { x, y };
    }

    void Input::on_mouse_button_press(const MouseButtonPressEvent &event)
    {
        const MouseCode mouse_code = event.mouse_code();
        m_mouse_buttons[mouse_code] = true;
    }

    void Input::on_mouse_button_release(const MouseButtonReleaseEvent &event)
    {
        const MouseCode mouse_code = event.mouse_code();
        m_mouse_buttons[mouse_code] = false;
    }

    void Input::on_key_press(const KeyPressEvent &event)
    {
        const KeyCode key_code = event.key_code();
        m_keys[key_code] = true;
    }

    void Input::on_key_release(const KeyReleaseEvent &event)
    {
        const KeyCode key_code = event.key_code();
        m_keys[key_code] = false;
    }
} // namespace he
