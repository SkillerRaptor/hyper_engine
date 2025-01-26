/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/texture.hpp"

namespace hyper_engine
{
    Texture::Texture(const TextureDescriptor &descriptor)
        : m_label(descriptor.label)
        , m_width(descriptor.width)
        , m_height(descriptor.height)
        , m_depth(descriptor.depth)
        , m_array_size(descriptor.array_size)
        , m_mip_levels(descriptor.mip_levels)
        , m_format(descriptor.format)
        , m_dimension(descriptor.dimension)
        , m_usage(descriptor.usage)
    {
    }

    std::string_view Texture::label() const
    {
        return m_label;
    }

    uint32_t Texture::width() const
    {
        return m_width;
    }

    uint32_t Texture::height() const
    {
        return m_height;
    }

    uint32_t Texture::depth() const
    {
        return m_depth;
    }

    uint32_t Texture::array_size() const
    {
        return m_array_size;
    }

    uint32_t Texture::mip_levels() const
    {
        return m_mip_levels;
    }

    Format Texture::format() const
    {
        return m_format;
    }

    Dimension Texture::dimension() const
    {
        return m_dimension;
    }

    BitFlags<TextureUsage> Texture::usage() const
    {
        return m_usage;
    }
} // namespace hyper_engine