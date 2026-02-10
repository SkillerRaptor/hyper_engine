/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <span>
#include <string>

#include <hyper_core/bit_flags.hpp>
#include <hyper_core/key.hpp>
#include <hyper_core/memory.hpp>
#include <hyper_core/types.hpp>

#include "hyper_rhi/forward.hpp"

namespace he
{
    enum class BufferUsage : u8
    {
        None = 0,
        TransferSrc = 1 << 0,
        TransferDst = 1 << 1,
        Index = 1 << 2,
        Indirect = 1 << 3,
        Storage = 1 << 4,
        ShaderResource = 1 << 5,
    };

    struct BufferDescriptor
    {
        std::optional<std::string_view> label = std::nullopt;
        u32 size = 0;
        BitFlags<BufferUsage> usage = BufferUsage::None;
        std::span<const u8> initial_data;
    };

    class Buffer
    {
    public:
        Buffer(Key<GraphicsDevice>, RefPtr<void> internal_state, const BufferDescriptor &desc)
            : m_internal_state(std::move(internal_state))
            , m_size(desc.size)
            , m_usage(desc.usage)
        {
        }

        HE_ALWAYS_INLINE const void *internal_state(Key<GraphicsDevice>) const { return m_internal_state.get(); }

        HE_ALWAYS_INLINE u32 size() const { return m_size; }
        HE_ALWAYS_INLINE BitFlags<BufferUsage> usage() const { return m_usage; }

    private:
        RefPtr<void> m_internal_state = nullptr;

        u32 m_size = 0;
        BitFlags<BufferUsage> m_usage = BufferUsage::None;
    };
} // namespace he
