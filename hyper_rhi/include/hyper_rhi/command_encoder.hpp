/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/prerequisites.hpp>

namespace he
{
    class CommandEncoder
    {
    public:
        virtual ~CommandEncoder() = default;

    protected:
        virtual void acquire() = 0;
        virtual void submit() = 0;

        HE_ALWAYS_INLINE void set_ready(const bool ready) { m_ready = ready; }

    protected:
        bool m_ready = false;
    };
} // namespace he
