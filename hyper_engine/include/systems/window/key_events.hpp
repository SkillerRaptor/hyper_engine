/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "systems/window/key_codes.hpp"

class KeyPressEvent
{
public:
    explicit KeyPressEvent(KeyCode);

    KeyCode key_code() const { return m_key_code; }

private:
    KeyCode m_key_code { KeyCode::Unknown };
};

class KeyReleaseEvent
{
public:
    explicit KeyReleaseEvent(KeyCode);

    KeyCode key_code() const { return m_key_code; }

private:
    KeyCode m_key_code { KeyCode::Unknown };
};
