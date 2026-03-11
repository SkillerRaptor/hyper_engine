/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>

#include <hyper_core/memory.hpp>
#include <hyper_core/prerequisites.hpp>
#include <hyper_core/types.hpp>

#include "hyper_rhi/definitions.hpp"
#include "hyper_rhi/resource.hpp"

namespace he {

struct TextureViewDescriptor {
    std::optional<std::string_view> label = std::nullopt;
    Texture texture;
    ViewDimension dimension = ViewDimension::D2;
    u32 base_mip_level = 0;
    std::optional<u32> mip_levels = std::nullopt;
    u32 base_array_layer = 0;
    std::optional<u32> array_layers = std::nullopt;
};

class TextureView : public Resource {
private:
    friend class GraphicsDevice;

public:
    HE_ALWAYS_INLINE Texture texture() const { return m_texture; }
    HE_ALWAYS_INLINE ViewDimension dimension() const { return m_dimension; }
    HE_ALWAYS_INLINE u32 base_mip_level() const { return m_base_mip_level; }
    HE_ALWAYS_INLINE u32 mip_levels() const { return m_mip_levels; }
    HE_ALWAYS_INLINE u32 base_array_layer() const { return m_base_array_layer; }
    HE_ALWAYS_INLINE u32 array_layers() const { return m_array_layers; }

private:
    TextureView(RefPtr<void> internal_state, const TextureViewDescriptor &desc)
        : Resource(std::move(internal_state))
        , m_texture(desc.texture)
        , m_dimension(desc.dimension)
        , m_base_mip_level(desc.base_mip_level)
        , m_mip_levels(desc.mip_levels.value_or(desc.texture.mip_levels()))
        , m_base_array_layer(desc.base_array_layer)
        , m_array_layers(desc.array_layers.value_or(desc.texture.extent().depth))
    {
    }

private:
    Texture m_texture;
    ViewDimension m_dimension = ViewDimension::D2;
    u32 m_base_mip_level = 0;
    u32 m_mip_levels = 1;
    u32 m_base_array_layer = 0;
    u32 m_array_layers = 1;
};

} // namespace he
