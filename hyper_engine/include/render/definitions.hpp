/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string>
#include <vector>

#include "core/bit_flags.hpp"
#include "core/id.hpp"
#include "core/types.hpp"
#include "render/resource_handle.hpp"

enum class ShaderType : u8
{
    None,
    Compute,
    Fragment,
    Vertex
};

enum class AddressMode : u8
{
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
    MirrorClampToEdge
};

enum class BorderColor : u8
{
    TransparentBlack,
    OpaqueBlack,
    OpaqueWhite,
};

enum class CompareOperation : u8
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

enum class Filter : u8
{
    Nearest,
    Linear,
};

enum class Dimension : u8
{
    Unknown,
    D1,
    D2,
    D3,
};

enum class ViewDimension : u8
{
    Unknown,
    D1,
    D2,
    D2Array,
    D3,
    Cube,
};

enum class Format : u8
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

enum class BlendFactor : u8
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

enum class BlendOperation : u8
{
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
};

enum class ColorWrites : u8
{
    R = 1 << 0,
    G = 1 << 1,
    B = 1 << 2,
    A = 1 << 3,
    Color = R | G | B,
    All = R | G | B | A,
};

HE_ENABLE_BIT_FLAGS(ColorWrites);

enum class Face : u8
{
    None,
    Front,
    Back
};

enum class FrontFace : u8
{
    CounterClockwise,
    Clockwise,
};

enum class LoadOperation : u8
{
    Clear,
    Load,
    DontCare,
};

enum class PolygonMode : u8
{
    Fill,
    Line,
    Point,
};

enum class PrimitiveTopology : u8
{
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
    TriangleFan,
};

enum class StoreOperation : u8
{
    None,
    Store,
    DontCare,
};

struct Extent2d
{
    u32 width { 0 };
    u32 height { 0 };
};

struct Extent3d
{
    u32 width { 0 };
    u32 height { 0 };
    u32 depth { 0 };
};

struct Offset2d
{
    int32_t x { 0 };
    int32_t y { 0 };
};

struct Offset3d
{
    int32_t x { 0 };
    int32_t y { 0 };
    int32_t z { 0 };
};

enum class BufferUsage : u8
{
    None = 0,
    Index = 1 << 0,
    Indirect = 1 << 1,
    Storage = 1 << 2,
    Resource = 1 << 3,
};

HE_ENABLE_BIT_FLAGS(BufferUsage);

enum class TextureUsage : u8
{
    None = 0,
    RenderAttachment = 1 << 0,
    Storage = 1 << 1,
    Resource = 1 << 2,
};

HE_ENABLE_BIT_FLAGS(TextureUsage);

struct Operations
{
    LoadOperation load_op { LoadOperation::Clear };
    StoreOperation store_op { StoreOperation::Store };
};

struct PrimitiveState
{
    PrimitiveTopology topology { PrimitiveTopology::TriangleList };
    FrontFace front_face { FrontFace::CounterClockwise };
    Face cull_mode { Face::None };
    PolygonMode polygon_mode { PolygonMode::Fill };
};

struct BlendState
{
    bool enable { false };
    BlendFactor src_factor { BlendFactor::One };
    BlendFactor dst_factor { BlendFactor::Zero };
    BlendOperation operation { BlendOperation::Add };
    BlendFactor alpha_src_factor { BlendFactor::One };
    BlendFactor alpha_dst_factor { BlendFactor::Zero };
    BlendOperation alpha_operation { BlendOperation::Add };
    ColorWrites color_writes { ColorWrites::All };
};

struct ColorAttachmentState
{
    Format format { Format::Unknown };
    BlendState blend_state {};
};

struct DepthBiasState
{
    bool enable { false };
    float constant { 0.0f };
    float clamp { 0.0f };
    float slope { 0.0f };
};

struct DepthStencilState
{
    bool depth_test_enable { false };
    bool depth_write_enable { false };
    Format depth_format { Format::Unknown };
    CompareOperation depth_compare_operation { CompareOperation::Less };
    DepthBiasState depth_bias_state {};
};

HE_DEFINE_ID(Buffer);
HE_DEFINE_ID(Shader);
HE_DEFINE_ID(Sampler);
HE_DEFINE_ID(Texture);
HE_DEFINE_ID(TextureView);
HE_DEFINE_ID(PipelineLayout);
HE_DEFINE_ID(ComputePipeline);
HE_DEFINE_ID(RenderPipeline);

struct BufferDescriptor
{
    std::optional<std::string> label { std::nullopt };
    u64 size { 0 };
    BufferUsage usage { BufferUsage::None };
    std::optional<ResourceHandle> handle { std::nullopt };
};

struct ShaderDescriptor
{
    std::optional<std::string> label { std::nullopt };
    ShaderType type { ShaderType::None };
    std::string entry {};
    std::string path {};
};

struct SamplerDescriptor
{
    std::optional<std::string> label { std::nullopt };
    Filter mag_filter { Filter::Linear };
    Filter min_filter { Filter::Linear };
    Filter mipmap_filter { Filter::Linear };
    AddressMode address_mode_u { AddressMode::ClampToEdge };
    AddressMode address_mode_v { AddressMode::ClampToEdge };
    AddressMode address_mode_w { AddressMode::ClampToEdge };
    CompareOperation compare_operation { CompareOperation::Less };
    float min_lod { 0.0f };
    float max_lod { 1.0f };
    BorderColor border_color { BorderColor::TransparentBlack };
    std::optional<ResourceHandle> handle { std::nullopt };
};

struct TextureDescriptor
{
    std::optional<std::string> label { std::nullopt };
    Extent3d extent {};
    u32 mip_levels { 1 };
    Format format { Format::Unknown };
    Dimension dimension { Dimension::Unknown };
    TextureUsage usage { TextureUsage::None };
};

struct TextureViewDescriptor
{
    std::optional<std::string> label { std::nullopt };
    TextureId texture {};
    ViewDimension dimension { ViewDimension::Unknown };
    u32 base_mip_level { 0 };
    u32 mip_levels { 1 };
    u32 base_array_layer { 0 };
    u32 array_layers { 1 };
    std::optional<ResourceHandle> handle { std::nullopt };
};

struct PipelineLayoutDescriptor
{
    std::optional<std::string> label { std::nullopt };
    u32 push_constant_size { 0 };
};

struct ComputePipelineDescriptor
{
    std::optional<std::string> label { std::nullopt };
    PipelineLayoutId layout {};
    ShaderId shader {};
};

struct RenderPipelineDescriptor
{
    std::optional<std::string> label { std::nullopt };
    PipelineLayoutId layout {};
    ShaderId vertex_shader {};
    ShaderId fragment_shader {};
    PrimitiveState primitive_state {};
    std::vector<ColorAttachmentState> color_attachment_states {};
    std::optional<DepthStencilState> depth_stencil_state { std::nullopt };
};

// Commands
HE_DEFINE_ID(CommandBuffer);

struct PassLabelColor
{
    uint8_t r { 255 };
    uint8_t g { 255 };
    uint8_t b { 255 };
};

struct PassLabel
{
    std::string name {};
    PassLabelColor color {};
};

struct BufferTargetDescriptor
{
    BufferId buffer {};
    u64 offset { 0 };
};

struct TextureTargetDescriptor
{
    TextureId texture {};
    Offset3d offset {};
};

struct ComputePassDescriptor
{
    std::optional<PassLabel> label { std::nullopt };
};

struct ColorAttachment
{
    TextureViewId view {};
    Operations operations {};
};

struct DepthStencilAttachment
{
    TextureViewId view {};
    Operations depth_operations {};
    // FIXME: Add stencil operation
};

struct RenderPassDescriptor
{
    std::optional<PassLabel> label { std::nullopt };
    std::vector<ColorAttachment> color_attachments {};
    std::optional<DepthStencilAttachment> depth_stencil_attachment { std::nullopt };
};
