/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <unordered_map>

#include <glm/vec2.hpp>

#include "core/prerequisites.hpp"
#include "platform/key_events.hpp"
#include "platform/mouse_events.hpp"

namespace he
{
    class EventLoop;

    class Input
    {
    public:
        explicit Input(EventLoop &);
        ~Input() = default;

        Input(const Input &) = delete;
        Input &operator=(const Input &) = delete;

        Input(Input &&) noexcept = default;
        Input &operator=(Input &&) noexcept = default;

        bool is_key_pressed(KeyCode) const;
        bool is_mouse_button_pressed(MouseCode) const;

        HE_ALWAYS_INLINE glm::vec2 mouse_position() const { return m_mouse_position; }

    private:
        void on_mouse_move(const MouseMoveEvent &event);
        void on_mouse_button_press(const MouseButtonPressEvent &event);
        void on_mouse_button_release(const MouseButtonReleaseEvent &event);
        void on_key_press(const KeyPressEvent &event);
        void on_key_release(const KeyReleaseEvent &event);

    private:
        std::unordered_map<KeyCode, bool> m_keys ;
        std::unordered_map<MouseCode, bool> m_mouse_buttons;
        glm::vec2 m_mouse_position = { 0.0f, 0.0f };
    };
} // namespace he
