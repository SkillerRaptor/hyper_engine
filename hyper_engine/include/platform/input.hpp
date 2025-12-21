/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>

#include <glm/vec2.hpp>

#include "core/prerequisites.hpp"
#include "platform/key_codes.hpp"
#include "platform/mouse_codes.hpp"

namespace he
{
    class Input
    {
    public:
        void update();

        bool is_key_pressed(KeyCode) const;
        bool is_mouse_button_pressed(MouseCode) const;

        HE_ALWAYS_INLINE glm::vec2 mouse_position() const { return m_mouse_position; }

    private:
        std::array<bool, 512> m_key_states = { };
        u32 m_mouse_state = 0;
        glm::vec2 m_mouse_position = { 0.0f, 0.0f };
    };
} // namespace he
