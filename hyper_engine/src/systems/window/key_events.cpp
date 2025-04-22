/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "systems/window/key_events.hpp"

KeyPressEvent::KeyPressEvent(const KeyCode key_code)
    : m_key_code(key_code)
{
}

KeyCode KeyPressEvent::key_code() const
{
    return m_key_code;
}

KeyReleaseEvent::KeyReleaseEvent(const KeyCode key_code)
    : m_key_code(key_code)
{
}

KeyCode KeyReleaseEvent::key_code() const
{
    return m_key_code;
}