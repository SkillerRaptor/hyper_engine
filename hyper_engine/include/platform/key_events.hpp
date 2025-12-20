/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/prerequisites.hpp"
#include "platform/key_codes.hpp"

namespace he
{
    class KeyPressEvent
    {
    public:
        explicit KeyPressEvent(const KeyCode key_code)
            : m_key_code(key_code)
        {
        }

        HE_ALWAYS_INLINE KeyCode key_code() const { return m_key_code; }

    private:
        KeyCode m_key_code = KeyCode::Unknown;
    };

    class KeyReleaseEvent
    {
    public:
        explicit KeyReleaseEvent(const KeyCode key_code)
            : m_key_code(key_code)
        {
        }

        HE_ALWAYS_INLINE KeyCode key_code() const { return m_key_code; }

    private:
        KeyCode m_key_code = KeyCode::Unknown;
    };
} // namespace he
