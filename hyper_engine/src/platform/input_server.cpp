/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "platform/input_server.hpp"

#include "core/logger.hpp"
#include "platform/event_server.hpp"

std::unique_ptr<InputServer> InputServer::create(EventServer &event_server)
{
    HE_INFO("Initialized input server");
    return std::unique_ptr<InputServer>(new InputServer(event_server));
}

InputServer::~InputServer() { HE_INFO("Shutdown input server"); }

// FIXME: Don't pass `EventServer` to make it more modular from the rest of the engine
InputServer::InputServer(EventServer &event_server)
{
    event_server.subscribe<MouseMoveEvent>(HE_BIND_FUNCTION(on_mouse_move));
    event_server.subscribe<MouseButtonPressEvent>(HE_BIND_FUNCTION(on_mouse_button_press));
    event_server.subscribe<MouseButtonReleaseEvent>(HE_BIND_FUNCTION(on_mouse_button_release));
    event_server.subscribe<KeyPressEvent>(HE_BIND_FUNCTION(on_key_press));
    event_server.subscribe<KeyReleaseEvent>(HE_BIND_FUNCTION(on_key_release));
}

bool InputServer::is_key_pressed(const KeyCode key_code) const
{
    return m_keys.contains(key_code) ? m_keys.at(key_code) : false;
}

bool InputServer::is_mouse_button_pressed(const MouseCode mouse_code) const
{
    return m_mouse_buttons.contains(mouse_code) ? m_mouse_buttons.at(mouse_code) : false;
}

void InputServer::on_mouse_move(const MouseMoveEvent &event)
{
    const float x = event.x();
    const float y = event.y();

    m_mouse_position = { x, y };
}

void InputServer::on_mouse_button_press(const MouseButtonPressEvent &event)
{
    const MouseCode mouse_code = event.mouse_code();
    m_mouse_buttons[mouse_code] = true;
}

void InputServer::on_mouse_button_release(const MouseButtonReleaseEvent &event)
{
    const MouseCode mouse_code = event.mouse_code();
    m_mouse_buttons[mouse_code] = false;
}

void InputServer::on_key_press(const KeyPressEvent &event)
{
    const KeyCode key_code = event.key_code();
    m_keys[key_code] = true;
}

void InputServer::on_key_release(const KeyReleaseEvent &event)
{
    const KeyCode key_code = event.key_code();
    m_keys[key_code] = false;
}
