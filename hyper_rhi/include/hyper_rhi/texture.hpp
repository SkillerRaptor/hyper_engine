/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

namespace hyper_rhi
{
    enum class TextureFormat
    {
        Unknown,
        // TODO: Add more texture formats
    };

    enum class TextureDimension
    {
        Unknown,
        Texture1D,
        Texture1DArray,
        Texture2D,
        Texture2DArray,
        Texture3D,
    };

    struct TextureDescriptor
    {
        std::string label;

        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
        uint32_t array_size = 1;
        uint32_t mip_levels = 1;
        uint32_t sample_count = 1;
        uint32_t sample_quality = 0;
        TextureFormat format = TextureFormat::Unknown;
        TextureDimension dimension = TextureDimension::Unknown;
    };

    class Texture
    {
    public:
        virtual ~Texture() = default;
    };
} // namespace hyper_rhi
