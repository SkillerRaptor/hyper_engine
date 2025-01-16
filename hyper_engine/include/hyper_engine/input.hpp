/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

#include "hyper_windowing/forward.hpp"
#include "hyper_windowing/key_codes.hpp"
#include "hyper_windowing/mouse_codes.hpp"

namespace hyper_engine
{
    class Input
    {
    public:
        Input();

        // FIXME: Add unsubscription event handlers
        ~Input() = default;

        bool is_key_pressed(KeyCode key_code) const;
        bool is_mouse_button_pressed(MouseCode mouse_code) const;

        glm::vec2 mouse_position() const;

    private:
        std::unordered_map<KeyCode, bool> m_keys;
        std::unordered_map<MouseCode, bool> m_mouse_buttons;
        glm::vec2 m_mouse_position = {0.0, 0.0};
    };
} // namespace hyper_engine