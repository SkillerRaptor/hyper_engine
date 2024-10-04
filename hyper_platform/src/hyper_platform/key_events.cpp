/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_platform/key_events.hpp"

namespace hyper_platform
{
    KeyPressedEvent::KeyPressedEvent(const KeyCode key_code)
        : m_key_code(key_code)
    {
    }

    KeyCode KeyPressedEvent::key_code() const
    {
        return m_key_code;
    }

    KeyReleasedEvent::KeyReleasedEvent(const KeyCode key_code)
        : m_key_code(key_code)
    {
    }

    KeyCode KeyReleasedEvent::key_code() const
    {
        return m_key_code;
    }
} // namespace hyper_platform
