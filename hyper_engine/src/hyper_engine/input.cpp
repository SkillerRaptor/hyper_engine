/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/input.hpp"

namespace hyper_engine
{
    Input::Input()
    {
    }

    bool Input::is_key_pressed(const KeyCode key_code) const
    {
        return m_keys.contains(key_code) ? m_keys.at(key_code) : false;
    }

    bool Input::is_mouse_button_pressed(const MouseCode mouse_code) const
    {
        return m_mouse_buttons.contains(mouse_code) ? m_mouse_buttons.at(mouse_code) : false;
    }

    glm::vec2 Input::mouse_position() const
    {
        return m_mouse_position;
    }
} // namespace hyper_engine