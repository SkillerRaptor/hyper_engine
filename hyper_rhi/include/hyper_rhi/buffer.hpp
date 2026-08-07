/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <span>
#include <string_view>

#include <hyper_core/bit_flags.hpp>
#include <hyper_core/memory.hpp>
#include <hyper_core/span.hpp>
#include <hyper_core/types.hpp>

#include "hyper_rhi/resource.hpp"

namespace he {

enum class BufferUsage : u8 {
    None = 0,
    TransferSrc = 1 << 0,
    TransferDst = 1 << 1,
    Index = 1 << 2,
    Indirect = 1 << 3,
    Storage = 1 << 4,
    ShaderResource = 1 << 5,
};

struct BufferDescriptor {
    std::optional<std::string_view> label = std::nullopt;
    u32 size = 0;
    BitFlags<BufferUsage> usage = BufferUsage::None;
    ReadonlySpan<u8> initial_data = { };
};

class Buffer : public Resource {
private:
    friend class GraphicsDevice;

public:
    u32 size() const { return m_size; }
    BitFlags<BufferUsage> usage() const { return m_usage; }

private:
    Buffer(RefPtr<void> internal_state, const BufferDescriptor &desc)
        : Resource(std::move(internal_state))
        , m_size(desc.size)
        , m_usage(desc.usage)
    {
    }

private:
    u32 m_size = 0;
    BitFlags<BufferUsage> m_usage = BufferUsage::None;
};

} // namespace he
