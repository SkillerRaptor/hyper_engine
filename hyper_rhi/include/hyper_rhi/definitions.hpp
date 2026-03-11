/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/types.hpp>

namespace he {

enum class AddressMode : u8 {
    Repeat = 0,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
    MirrorClampToEdge
};

enum class BlendFactor : u8 {
    Zero = 0,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,
    SrcAlphaSaturate,
    Src1Color,
    OneMinusSrc1Color,
    Src1Alpha,
    OneMinusSrc1Alpha,
};

enum class BlendOperation : u8 {
    Add = 0,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
};

enum class BorderColor : u8 {
    TransparentBlack = 0,
    OpaqueBlack,
    OpaqueWhite,
};

enum class ColorWrites : u8 {
    R = 1 << 0,
    G = 1 << 1,
    B = 1 << 2,
    A = 1 << 3,
    Color = R | G | B,
    All = R | G | B | A,
};

enum class CompareOperation : u8 {
    Never = 0,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

enum class Dimension : u8 {
    D1 = 0,
    D2,
    D3,
};

struct Extent2d {
    u32 width = 0;
    u32 height = 0;
};

struct Extent3d {
    u32 width = 0;
    u32 height = 0;
    u32 depth = 0;
};

enum class Face : u8 {
    None = 0,
    Front,
    Back
};

enum class Filter : u8 {
    Nearest = 0,
    Linear,
};

enum class Format : u8 {
    None = 0,

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

enum class FrontFace : u8 {
    CounterClockwise = 0,
    Clockwise,
};

enum class LoadOperation : u8 {
    Clear = 0,
    Load,
    DontCare,
};

struct Offset2d {
    i32 x = 0;
    i32 y = 0;
};

struct Offset3d {
    i32 x = 0;
    i32 y = 0;
    i32 z = 0;
};

enum class PolygonMode : u8 {
    Fill = 0,
    Line,
    Point,
};

enum class PrimitiveTopology : u8 {
    PointList = 0,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
    TriangleFan,
};

enum class ShaderType : u8 {
    Compute = 0,
    Fragment,
    Vertex
};

enum class StoreOperation : u8 {
    None = 0,
    Store,
    DontCare,
};

enum class ViewDimension : u8 {
    D1 = 0,
    D2,
    D2Array,
    D3,
    Cube,
};

} // namespace he
