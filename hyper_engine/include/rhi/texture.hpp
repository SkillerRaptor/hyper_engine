/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>

#include <hyper_core/bit_flags.hpp>
#include <hyper_core/types.hpp>

#include "rhi/definitions.hpp"

namespace he
{
    class Texture;

    enum class TextureUsage : u8
    {
        None = 0,
        RenderAttachment = 1 << 0,
        Storage = 1 << 1,
        Resource = 1 << 2,
    };

    struct TextureDescriptor
    {
        std::optional<std::string_view> label = std::nullopt;
        Extent3d extent;
        u32 mip_levels = 1;
        u32 sample_count = 1;
        Format format = Format::None;
        Dimension dimension = Dimension::D2;
        BitFlags<TextureUsage> usage = TextureUsage::None;
    };

    struct TextureViewDescriptor
    {
        std::optional<std::string_view> label = std::nullopt;
        RefPtr<Texture> texture = nullptr;
        ViewDimension dimension = ViewDimension::D2;
        u32 base_mip_level = 0;
        u32 mip_levels = 1;
        u32 base_array_layer = 0;
        u32 array_layers = 1;
    };

    class Texture
    {
    public:
        virtual ~Texture() = default;

        HE_ALWAYS_INLINE Extent3d extent() const { return m_extent; }

        HE_ALWAYS_INLINE u32 mip_levels() const { return m_mip_levels; }

        HE_ALWAYS_INLINE u32 sample_count() const { return m_sample_count; }

        HE_ALWAYS_INLINE Format format() const { return m_format; }

        HE_ALWAYS_INLINE Dimension dimension() const { return m_dimension; }

        HE_ALWAYS_INLINE BitFlags<TextureUsage> usage() const { return m_usage; }

    protected:
        explicit Texture(const TextureDescriptor &desc)
            : m_extent(desc.extent)
            , m_mip_levels(desc.mip_levels)
            , m_sample_count(desc.sample_count)
            , m_format(desc.format)
            , m_dimension(desc.dimension)
            , m_usage(desc.usage)
        {
        }

    protected:
        Extent3d m_extent;
        u32 m_mip_levels = 1;
        u32 m_sample_count = 1;
        Format m_format = Format::None;
        Dimension m_dimension = Dimension::D2;
        BitFlags<TextureUsage> m_usage = TextureUsage::None;
    };

    class TextureView
    {
    public:
        virtual ~TextureView() = default;

        HE_ALWAYS_INLINE RefPtr<Texture> texture() const { return m_texture; }

        HE_ALWAYS_INLINE ViewDimension dimension() const { return m_dimension; }

        HE_ALWAYS_INLINE u32 base_mip_level() const { return m_base_mip_level; }

        HE_ALWAYS_INLINE u32 mip_levels() const { return m_mip_levels; }

        HE_ALWAYS_INLINE u32 base_array_layer() const { return m_base_array_layer; }

        HE_ALWAYS_INLINE u32 array_layers() const { return m_array_layers; }

    protected:
        explicit TextureView(const TextureViewDescriptor &desc)
            : m_texture(desc.texture)
            , m_dimension(desc.dimension)
            , m_base_mip_level(desc.base_mip_level)
            , m_mip_levels(desc.mip_levels)
            , m_base_array_layer(desc.base_array_layer)
            , m_array_layers(desc.array_layers)
        {
        }

    protected:
        RefPtr<Texture> m_texture = nullptr;
        ViewDimension m_dimension = ViewDimension::D2;
        u32 m_base_mip_level = 0;
        u32 m_mip_levels = 1;
        u32 m_base_array_layer = 0;
        u32 m_array_layers = 1;
    };
} // namespace he
