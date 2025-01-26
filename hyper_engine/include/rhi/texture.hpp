/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

#include <hyper_core/bit_flags.hpp>

#include "hyper_rhi/dimension.hpp"
#include "hyper_rhi/format.hpp"

namespace hyper_engine
{
    enum class TextureUsage : uint8_t
    {
        None = 0,
        Storage = 1 << 0,
        RenderAttachment = 1 << 1,
        ShaderResource = 1 << 2,
    };

    struct TextureDescriptor
    {
        std::string label;
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
        uint32_t array_size = 1;
        uint32_t mip_levels = 1;
        Format format = Format::Unknown;
        Dimension dimension = Dimension::Unknown;
        BitFlags<TextureUsage> usage = TextureUsage::None;
    };

    class Texture
    {
    public:
        virtual ~Texture() = default;

        std::string_view label() const;
        uint32_t width() const;
        uint32_t height() const;
        uint32_t depth() const;
        uint32_t array_size() const;
        uint32_t mip_levels() const;
        Format format() const;
        Dimension dimension() const;
        BitFlags<TextureUsage> usage() const;

    protected:
        explicit Texture(const TextureDescriptor &descriptor);

    protected:
        std::string m_label;
        uint32_t m_width = 1;
        uint32_t m_height = 1;
        uint32_t m_depth = 1;
        uint32_t m_array_size = 1;
        uint32_t m_mip_levels = 1;
        Format m_format = Format::Unknown;
        Dimension m_dimension = Dimension::Unknown;
        BitFlags<TextureUsage> m_usage = TextureUsage::None;
    };
} // namespace hyper_engine
