/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_platform/input.hpp"

#include <SDL3/SDL.h>

namespace he
{
    void Input::update()
    {
        const bool *key_state = SDL_GetKeyboardState(nullptr);
        memcpy(m_key_states.data(), key_state, m_key_states.size() * sizeof(bool));

        m_mouse_state = SDL_GetMouseState(&m_mouse_position.first, &m_mouse_position.second);
    }

    bool Input::is_key_pressed(const KeyCode key_code) const { return m_key_states[static_cast<u32>(key_code)]; }

    bool Input::is_mouse_button_pressed(const MouseCode mouse_code) const
    {
        return (SDL_BUTTON_MASK(static_cast<u32>(mouse_code)) & m_mouse_state) > 0;
    }
} // namespace he
