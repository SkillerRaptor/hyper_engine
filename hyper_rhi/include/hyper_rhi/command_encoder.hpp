/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/key.hpp>

#include "hyper_core/prerequisites.hpp"
#include "hyper_rhi/forward.hpp"

namespace he
{
    class BackendCommandEncoder
    {
    public:
        virtual ~BackendCommandEncoder() = default;

        virtual void acquire() = 0;
        virtual void submit() = 0;
    };

    class CommandEncoder
    {
    public:
        CommandEncoder(Key<GraphicsDevice>, BackendCommandEncoder &backend_command_encoder)
            : m_backend_command_encoder(backend_command_encoder)
        {
        }

        virtual ~CommandEncoder() = default;

        CommandEncoder(CommandEncoder &&) noexcept = default;
        CommandEncoder &operator=(CommandEncoder &&) noexcept = default;

        CommandEncoder(const CommandEncoder &) = delete;
        CommandEncoder &operator=(const CommandEncoder &) = delete;

        HE_ALWAYS_INLINE BackendCommandEncoder &backend_command_encoder(Key<GraphicsDevice>) const
        {
            return m_backend_command_encoder;
        }

    private:
        BackendCommandEncoder &m_backend_command_encoder;
    };
} // namespace he
