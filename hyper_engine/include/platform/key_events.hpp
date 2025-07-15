/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "platform/key_codes.hpp"

class KeyPressEvent
{
public:
    explicit KeyPressEvent(const KeyCode key_code)
        : m_key_code { key_code }
    {
    }

    KeyCode key_code() const { return m_key_code; }

private:
    KeyCode m_key_code { KeyCode::Unknown };
};

class KeyReleaseEvent
{
public:
    explicit KeyReleaseEvent(const KeyCode key_code)
        : m_key_code { key_code }
    {
    }

    KeyCode key_code() const { return m_key_code; }

private:
    KeyCode m_key_code { KeyCode::Unknown };
};
