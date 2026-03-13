/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>

#include <hyper_core/bit_flags.hpp>
#include <hyper_core/memory.hpp>
#include <hyper_core/prerequisites.hpp>
#include <hyper_core/types.hpp>

#include "hyper_rhi/definitions.hpp"
#include "hyper_rhi/resource.hpp"

namespace he {

enum class TextureUsage : u8 {
    None = 0,
    TransferSrc = 1 << 0,
    TransferDst = 1 << 1,
    RenderAttachment = 1 << 2,
    Storage = 1 << 3,
    ShaderResource = 1 << 4,
};

struct TextureDescriptor {
    std::optional<std::string_view> label { std::nullopt };
    Extent3d extent {};
    u32 mip_levels { 1 };
    u32 sample_count { 1 };
    Format format { Format::None };
    Dimension dimension { Dimension::D2 };
    BitFlags<TextureUsage> usage { TextureUsage::None };
};

class Texture : public Resource {
private:
    friend class GraphicsDevice;

public:
    HE_ALWAYS_INLINE Extent3d extent() const { return m_extent; }
    HE_ALWAYS_INLINE u32 mip_levels() const { return m_mip_levels; }
    HE_ALWAYS_INLINE u32 sample_count() const { return m_sample_count; }
    HE_ALWAYS_INLINE Format format() const { return m_format; }
    HE_ALWAYS_INLINE Dimension dimension() const { return m_dimension; }
    HE_ALWAYS_INLINE BitFlags<TextureUsage> usage() const { return m_usage; }

private:
    Texture(RefPtr<void> internal_state, const TextureDescriptor &desc)
        : Resource(std::move(internal_state))
        , m_extent(desc.extent)
        , m_mip_levels(desc.mip_levels)
        , m_sample_count(desc.sample_count)
        , m_format(desc.format)
        , m_dimension(desc.dimension)
        , m_usage(desc.usage)
    {
    }

private:
    Extent3d m_extent {};
    u32 m_mip_levels { 1 };
    u32 m_sample_count { 1 };
    Format m_format { Format::None };
    Dimension m_dimension { Dimension::D2 };
    BitFlags<TextureUsage> m_usage { TextureUsage::None };
};

} // namespace he
