/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <span>

#include "core/bit_flags.hpp"
#include "core/resource_owner.hpp"
#include "systems/window_system.hpp"

using RS = class RenderSystem;

class RenderDriver;

class RenderSystem
{
private:
    static constexpr uint8_t s_frames_in_flight = 3;

public:
    // Buffer
    enum class BufferUsage : uint8_t
    {
        None = 0,
        Index = 1 << 0,
        Indirect = 1 << 1,
        Storage = 1 << 2,
        ShaderResource = 1 << 3,
    };

    HE_DEFINE_ID(Buffer);

    struct BufferDescriptor
    {
        std::optional<std::string> label;
        uint64_t byte_size = 0;
        BitFlags<BufferUsage> usage = BufferUsage::None;
    };

    struct Buffer
    {
        BufferDescriptor descriptor = {};
    };

    // Shader
    enum class ShaderType : uint8_t
    {
        None,
        Compute,
        Fragment,
        Vertex
    };

    struct ShaderDescriptor
    {
        std::optional<std::string> label;
        ShaderType type = ShaderType::None;
        std::string entry;
        std::string path;
    };

    HE_DEFINE_ID(Shader);

    struct Shader
    {
        ShaderDescriptor descriptor = {};
    };

    // Sampler
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

    HE_DEFINE_ID(Sampler);

    struct SamplerDescriptor
    {
        std::optional<std::string> label;
        Filter mag_filter = Filter::Linear;
        Filter min_filter = Filter::Linear;
        Filter mipmap_filter = Filter::Linear;
        AddressMode address_mode_u = AddressMode::ClampToEdge;
        AddressMode address_mode_v = AddressMode::ClampToEdge;
        AddressMode address_mode_w = AddressMode::ClampToEdge;
        float mip_lod_bias = 0.0;
        CompareOperation compare_operation = CompareOperation::Less;
        float min_lod = 0.0;
        float max_lod = 1.0;
        BorderColor border_color = BorderColor::TransparentBlack;
    };

    struct Sampler
    {
        SamplerDescriptor descriptor = {};
    };

    // Texture
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

    HE_DEFINE_ID(Texture);

    struct TextureDescriptor
    {
        std::optional<std::string> label;
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
        uint32_t array_size = 1;
        uint32_t mip_levels = 1;
        Format format = Format::Unknown;
        Dimension dimension = Dimension::Unknown;
        BitFlags<TextureUsage> usage = TextureUsage::None;
    };

    struct Texture
    {
        TextureDescriptor descriptor = {};
    };

    // Pipeline Layout
    HE_DEFINE_ID(PipelineLayout);

    struct PipelineLayoutDescriptor
    {
        std::optional<std::string> label;
        uint32_t push_constant_size = 0;
    };

    struct PipelineLayout
    {
        PipelineLayoutDescriptor descriptor = {};
    };

    // Compute Pipeline
    HE_DEFINE_ID(ComputePipeline);

    struct ComputePipelineDescriptor
    {
        std::optional<std::string> label;
        PipelineLayoutId layout;
        ShaderId shader;
    };

    struct ComputePipeline
    {
        ComputePipelineDescriptor descriptor = {};
    };

    // Render Pipeline
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

    HE_DEFINE_ID(RenderPipeline);

    struct RenderPipelineDescriptor
    {
        std::optional<std::string> label;
        PipelineLayoutId layout;
        ShaderId vertex_shader;
        ShaderId fragment_shader;
        std::vector<ColorAttachmentState> color_attachment_states;
        PrimitiveState primitive_state;
        DepthStencilState depth_stencil_state;
    };

    struct RenderPipeline
    {
        RenderPipelineDescriptor descriptor = {};
    };

    //

    struct SubresourceRange
    {
        uint32_t base_mip_level = 0;
        uint32_t mip_level_count = 1;
        uint32_t base_array_level = 0;
        uint32_t array_layer_count = 1;
    };

    struct Viewport
    {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
        float min_depth = 0.0f;
        float max_depth = 1.0f;
    };

    struct Scissor
    {
        int32_t x = 0;
        int32_t y = 0;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    // Commands
    HE_DEFINE_ID(CommandBuffer);

    struct CommandBuffer
    {
        uint32_t generation = 0;
        bool swapchain_texture_acquired = false;
        bool submitted = false;
    };

    HE_DEFINE_ID(ComputePass);

    struct ComputePass
    {
        CommandBufferId command_buffer;
        bool ended = false;
    };

    HE_DEFINE_ID(RenderPass);

    struct RenderPassDescriptor
    {
        TextureId texture;
    };

    struct RenderPass
    {
        CommandBufferId command_buffer;
        bool ended = false;
    };

public:
    ~RenderSystem();

    void initialize(const WindowSystem &window_system, WindowSystem::WindowId window);

    // Buffer
    BufferId create_buffer(const BufferDescriptor &descriptor);
    void destroy_buffer(BufferId id);

    // Shader
    ShaderId create_shader(const ShaderDescriptor &descriptor);
    void destroy_shader(ShaderId id);

    // Sampler
    SamplerId create_sampler(const SamplerDescriptor &descriptor);
    void destroy_sampler(SamplerId id);

    // Texture
    TextureId create_texture(const TextureDescriptor &descriptor);
    void destroy_texture(TextureId id);

    // Pipeline Layout
    PipelineLayoutId create_pipeline_layout(const PipelineLayoutDescriptor &descriptor);
    void destroy_pipeline_layout(PipelineLayoutId id);

    // Compute Pipeline
    ComputePipelineId create_compute_pipeline(const ComputePipelineDescriptor &descriptor);
    void destroy_compute_pipeline(ComputePipelineId id);

    // Render Pipeline
    RenderPipelineId create_render_pipeline(const RenderPipelineDescriptor &descriptor);
    void destroy_render_pipeline(RenderPipelineId id);

    // Commands
    CommandBufferId acquire_command_buffer() const;
    void submit_command_buffer(CommandBufferId id);

    TextureId acquire_swapchain_texture(CommandBufferId id);

    // Compute Pass
    ComputePassId begin_compute_pass(CommandBufferId id);
    void end_compute_pass(ComputePassId id);

    void bind_compute_pipeline(ComputePassId id, ComputePipelineId pipeline_id);

    // Render Pass
    RenderPassId begin_render_pass(CommandBufferId id, const RenderPassDescriptor &descriptor);
    void end_render_pass(RenderPassId id);

    void bind_render_pipeline(RenderPassId id, RenderPipelineId pipeline_id);

    void set_viewport(RenderPassId id, const Viewport &viewport);
    void set_scissor(RenderPassId id, const Scissor &scissor);
    void draw(RenderPassId id, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);

private:
    CommandBuffer *resolve_command_buffer(CommandBufferId id) const;
    ComputePass &resolve_compute_pass(ComputePassId id);
    RenderPass &resolve_render_pass(RenderPassId id);

private:
    RenderDriver *m_render_driver = nullptr;
    std::unordered_map<uint32_t, TextureId> m_swapchain_textures = {};
    std::array<CommandBuffer *, s_frames_in_flight> m_command_buffers = {};
    std::vector<ComputePass> m_compute_passes;
    std::vector<RenderPass> m_render_passes;

    uint32_t m_frame_index = 0;

    ResourceOwner<BufferId, Buffer *> m_buffers;
    ResourceOwner<ShaderId, Shader *> m_shaders;
    ResourceOwner<SamplerId, Sampler *> m_samplers;
    ResourceOwner<TextureId, Texture *> m_textures;
    ResourceOwner<PipelineLayoutId, PipelineLayout *> m_pipeline_layouts;
    ResourceOwner<ComputePipelineId, ComputePipeline *> m_compute_pipelines;
    ResourceOwner<RenderPipelineId, RenderPipeline *> m_render_pipelines;
};