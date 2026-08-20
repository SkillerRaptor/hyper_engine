/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>

#include "hyper_rhi/forward.hpp"
#include "hyper_rhi/types.hpp"

namespace he {

struct TextureViewDescriptor {
    std::optional<std::string_view> label = std::nullopt;
    Texture *texture = nullptr;
    ViewDimension dimension = ViewDimension::D2;
    u32 base_mip_level = 0;
    std::optional<u32> mip_levels = std::nullopt;
    u32 base_array_layer = 0;
    std::optional<u32> array_layers = std::nullopt;
};

class TextureView {
public:
    explicit TextureView(const TextureViewDescriptor &desc)
        : m_texture(desc.texture)
        , m_dimension(desc.dimension)
        , m_base_mip_level(desc.base_mip_level)
        , m_mip_levels(desc.mip_levels)
        , m_base_array_layer(desc.base_array_layer)
        , m_array_layers(desc.array_layers)
    {
    }

    virtual ~TextureView() = default;

    Texture *texture() const { return m_texture; }
    ViewDimension dimension() const { return m_dimension; }
    u32 base_mip_level() const { return m_base_mip_level; }
    std::optional<u32> mip_levels() const { return m_mip_levels; }
    u32 base_array_layer() const { return m_base_array_layer; }
    std::optional<u32> array_layers() const { return m_array_layers; }

private:
    Texture *m_texture = nullptr;
    ViewDimension m_dimension = ViewDimension::D2;
    u32 m_base_mip_level = 0;
    std::optional<u32> m_mip_levels = std::nullopt;
    u32 m_base_array_layer = 0;
    std::optional<u32> m_array_layers = std::nullopt;
};

} // namespace he
