/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string>

#include "core/bit_flags.hpp"

enum class BufferUsage : uint8_t
{
    None = 0,
    Index = 1 << 0,
    Indirect = 1 << 1,
    Storage = 1 << 2,
    ShaderResource = 1 << 3,
};

enum class ShaderType : uint8_t
{
    None,
    Compute,
    Fragment,
    Vertex
};

enum class Filter : uint8_t
{
    Nearest,
    Linear,
};

enum class AddressMode : uint8_t
{
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
    MirrorClampToEdge
};

enum class CompareOperation : uint8_t
{
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

enum class BorderColor : uint8_t
{
    TransparentBlack,
    OpaqueBlack,
    OpaqueWhite,
};

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

enum class Dimension : uint8_t
{
    Unknown,
    Texture1D,
    Texture1DArray,
    Texture2D,
    Texture2DArray,
    Texture3D,
};

enum class TextureUsage : uint8_t
{
    None = 0,
    Storage = 1 << 0,
    RenderAttachment = 1 << 1,
    ShaderResource = 1 << 2,
};

enum class BlendFactor : uint8_t
{
    Zero,
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

enum class BlendOperation : uint8_t
{
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
};

enum class ColorWrites : uint8_t
{
    R = 1 << 0,
    G = 1 << 1,
    B = 1 << 2,
    A = 1 << 3,
    Color = R | G | B,
    All = R | G | B | A,
};

struct BlendState
{
    bool blend_enable = false;
    BlendFactor src_blend_factor = BlendFactor::One;
    BlendFactor dst_blend_factor = BlendFactor::Zero;
    BlendOperation operation = BlendOperation::Add;
    BlendFactor alpha_src_blend_factor = BlendFactor::One;
    BlendFactor alpha_dst_blend_factor = BlendFactor::Zero;
    BlendOperation alpha_operation = BlendOperation::Add;
    BitFlags<ColorWrites> color_writes = ColorWrites::All;
};

struct ColorAttachmentState
{
    Format format = Format::Unknown;
    BlendState blend_state = {};
};

enum class PrimitiveTopology : uint8_t
{
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
    TriangleFan,
};

enum class FrontFace : uint8_t
{
    CounterClockwise,
    Clockwise,
};

enum class Face : uint8_t
{
    None,
    Front,
    Back
};

enum class PolygonMode : uint8_t
{
    Fill,
    Line,
    Point,
};

struct PrimitiveState
{
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    FrontFace front_face = FrontFace::CounterClockwise;
    Face cull_mode = Face::None;
    PolygonMode polygon_mode = PolygonMode::Fill;
};

struct DepthBiasState
{
    bool depth_bias_enable = false;
    float constant = 0.0f;
    float clamp = 0.0f;
    float slope = 0.0f;
};

struct DepthStencilState
{
    bool depth_test_enable = false;
    bool depth_write_enable = false;
    Format depth_format = Format::Unknown;
    CompareOperation depth_compare_operation = CompareOperation::Less;
    DepthBiasState depth_bias_state;
};

struct LabelColor
{
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
};

struct Label
{
    std::string name;
    LabelColor color;
};