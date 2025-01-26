/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace hyper_engine
{
    enum class Format : uint8_t
    {
        Unknown,

        R8Unorm,
        R8Snorm,
        R8Uint,
        R8Sint,
        R8Srgb,

        Rg8Unorm,
        Rg8Snorm,
        Rg8Uint,
        Rg8Sint,
        Rg8Srgb,

        Rgb8Unorm,
        Rgb8Snorm,
        Rgb8Uint,
        Rgb8Sint,
        Rgb8Srgb,

        Bgr8Unorm,
        Bgr8Snorm,
        Bgr8Uint,
        Bgr8Sint,
        Bgr8Srgb,

        Rgba8Unorm,
        Rgba8Snorm,
        Rgba8Uint,
        Rgba8Sint,
        Rgba8Srgb,

        Bgra8Unorm,
        Bgra8Snorm,
        Bgra8Uint,
        Bgra8Sint,
        Bgra8Srgb,

        R16Unorm,
        R16Snorm,
        R16Uint,
        R16Sint,
        R16Sfloat,

        Rg16Unorm,
        Rg16Snorm,
        Rg16Uint,
        Rg16Sint,
        Rg16Sfloat,

        Rgb16Unorm,
        Rgb16Snorm,
        Rgb16Uint,
        Rgb16Sint,
        Rgb16Sfloat,

        Rgba16Unorm,
        Rgba16Snorm,
        Rgba16Uint,
        Rgba16Sint,
        Rgba16Sfloat,

        R32Uint,
        R32Sint,
        R32Sfloat,

        Rg32Uint,
        Rg32Sint,
        Rg32Sfloat,

        Rgb32Uint,
        Rgb32Sint,
        Rgb32Sfloat,

        Rgba32Uint,
        Rgba32Sint,
        Rgba32Sfloat,

        R64Uint,
        R64Sint,
        R64Sfloat,

        Rg64Uint,
        Rg64Sint,
        Rg64Sfloat,

        Rgb64Uint,
        Rgb64Sint,
        Rgb64Sfloat,

        Rgba64Uint,
        Rgba64Sint,
        Rgba64Sfloat,

        D16Unorm,
        D32Sfloat,
        S8Uint,
        D16UnormS8Uint,
        D24UnormS8Uint,
        D32SfloatS8Uint
    };
} // namespace hyper_engine