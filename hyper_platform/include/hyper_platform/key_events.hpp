/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_platform/key_codes.hpp"

namespace hyper_platform
{
    class KeyPressedEvent
    {
    public:
        explicit KeyPressedEvent(KeyCode key_code);

        KeyCode key_code() const;

    private:
        KeyCode m_key_code;
    };

    class KeyReleasedEvent
    {
    public:
        explicit KeyReleasedEvent(KeyCode key_code);

        KeyCode key_code() const;

    private:
        KeyCode m_key_code;
    };
} // namespace hyper_platform
