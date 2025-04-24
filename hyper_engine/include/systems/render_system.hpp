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
#include "systems/render/render_driver_common.hpp"
#include "systems/window_system.hpp"

HE_DEFINE_ID(Buffer);
HE_DEFINE_ID(Shader);
HE_DEFINE_ID(Sampler);
HE_DEFINE_ID(Texture);
HE_DEFINE_ID(PipelineLayout);
HE_DEFINE_ID(ComputePipeline);
HE_DEFINE_ID(RenderPipeline);
HE_DEFINE_ID(CommandBuffer);
HE_DEFINE_ID(ComputePass);
HE_DEFINE_ID(RenderPass);

struct BufferDescriptor
{
    std::optional<std::string> label;
    uint64_t size = 0;
    BitFlags<BufferUsage> usage = BufferUsage::None;
};

struct ShaderDescriptor
{
    std::optional<std::string> label;
    ShaderType type = ShaderType::None;
    std::string entry;
    std::string path;
};

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

struct PipelineLayoutDescriptor
{
    std::optional<std::string> label;
    uint32_t push_constant_size = 0;
};

struct ComputePipelineDescriptor
{
    std::optional<std::string> label;
    PipelineLayoutId layout;
    ShaderId shader;
};

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

struct ComputePassDescriptor
{
    std::optional<Label> label;
};

struct ColorAttachment
{
    TextureId texture;
    Operations operations = {};
};

struct DepthStencilAttachment
{
    TextureId texture;
    Operations depth_operations;
    // FIXME: Add stencil operation
};

struct RenderPassDescriptor
{
    std::optional<Label> label;
    std::vector<ColorAttachment> color_attachments;
    std::optional<DepthStencilAttachment> depth_stencil_attachment;
};

class RenderDriver;

struct Buffer;
struct Shader;
struct Sampler;
struct Texture;
struct PipelineLayout;
struct ComputePipeline;
struct RenderPipeline;
struct CommandBuffer;
struct ComputePass;
struct RenderPass;

class RenderSystem
{
private:
    static constexpr uint8_t s_frames_in_flight = 3;

private:
    enum class ResourceTag
    {
        None,
        Buffer,
        Shader,
        Sampler,
        Texture,
        PipelineLayout,
        ComputePipeline,
        RenderPipeline,
    };

    struct Resource
    {
        ResourceTag tag = ResourceTag::None;
        const void *inner_resource = nullptr;
        uint8_t generation = 0;
    };

public:
    ~RenderSystem();

    void initialize(WindowSystem &window_system, WindowId window);

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
    CommandBufferId acquire_command_buffer();
    void submit_command_buffer(CommandBufferId id);

    // FIXME: Add an id system & add a system to prevent overriding in FiF, because memory changes are reflected immediately
    void bind_buffer(BufferId id, uint32_t slot) const;
    void bind_sampler(SamplerId id, uint32_t slot) const;
    void bind_texture(TextureId id, uint32_t slot) const;

    void clear_buffer(CommandBufferId id, BufferId buffer, size_t size, uint64_t offset) const;
    void write_buffer(CommandBufferId id, BufferId buffer, const void *data, size_t size, uint64_t offset);

    template <typename T>
    void write_buffer(const CommandBufferId id, const BufferId buffer, const T &data, const uint64_t offset)
    {
        write_buffer(id, buffer, &data, sizeof(T), offset);
    }

    void clear_texture(CommandBufferId id, TextureId texture, SubresourceRange subresource_range) const;
    void write_texture(
        CommandBufferId id,
        TextureId texture,
        Offset3d offset,
        Extent3d extent,
        uint32_t mip_level,
        uint32_t array_index,
        const void *data,
        size_t data_size,
        uint64_t data_offset);

    template <typename T>
    void write_texture(
        const CommandBufferId id,
        const TextureId texture,
        const Offset3d offset,
        const Extent3d extent,
        const uint32_t mip_level,
        const uint32_t array_index,
        const T &data,
        const uint64_t data_offset)
    {
        write_texture(id, texture, offset, extent, mip_level, array_index, &data, sizeof(T), data_offset);
    }

    void copy_buffer_to_buffer(CommandBufferId id, BufferId src, uint64_t src_offset, BufferId dst, uint64_t dst_offset, size_t size) const;
    void copy_buffer_to_texture(
        CommandBufferId id,
        BufferId src,
        uint64_t src_offset,
        TextureId dst,
        Offset3d dst_offset,
        Extent3d dst_extent,
        uint32_t dst_mip_level,
        uint32_t dst_array_index) const;
    void copy_texture_to_buffer(
        CommandBufferId id,
        TextureId src,
        Offset3d src_offset,
        Extent3d src_extent,
        uint32_t src_mip_level,
        uint32_t src_array_index,
        BufferId dst,
        uint64_t dst_offset) const;
    void copy_texture_to_texture(
        CommandBufferId id,
        TextureId src,
        Offset3d src_offset,
        uint32_t src_mip_level,
        uint32_t src_array_index,
        TextureId dst,
        Offset3d dst_offset,
        uint32_t dst_mip_level,
        uint32_t dst_array_index,
        Extent3d extent) const;

    TextureId acquire_swapchain_texture(CommandBufferId id);

    // Compute Pass
    ComputePassId begin_compute_pass(CommandBufferId id, const ComputePassDescriptor &descriptor);
    void end_compute_pass(ComputePassId id) const;

    void bind_pipeline(ComputePassId id, ComputePipelineId pipeline_id) const;
    void push_constants(ComputePassId id, const void *data, size_t data_size) const;

    template <typename T>
    void push_constants(const ComputePassId id, const T &data) const
    {
        push_constants(id, &data, sizeof(T));
    }

    // Render Pass
    RenderPassId begin_render_pass(CommandBufferId id, const RenderPassDescriptor &descriptor);
    void end_render_pass(RenderPassId id) const;

    void bind_pipeline(RenderPassId id, RenderPipelineId pipeline_id) const;
    void bind_index_buffer(RenderPassId id, BufferId buffer) const;
    void push_constants(RenderPassId id, const void *data, size_t data_size) const;

    template <typename T>
    void push_constants(const RenderPassId id, const T &data) const
    {
        push_constants(id, &data, sizeof(T));
    }

    void set_viewport(RenderPassId id, float x, float y, float width, float height, float min_depth, float max_depth) const;
    void set_scissor(RenderPassId id, int32_t x, int32_t y, uint32_t width, uint32_t height) const;
    void draw(RenderPassId id, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) const;
    void draw_indexed(
        RenderPassId id,
        uint32_t index_count,
        uint32_t instance_count,
        uint32_t first_index,
        int32_t vertex_offset,
        uint32_t first_instance) const;

private:
    CommandBuffer *resolve_command_buffer(CommandBufferId id) const;
    ComputePass *resolve_compute_pass(ComputePassId id) const;
    RenderPass *resolve_render_pass(RenderPassId id) const;

private:
    RenderDriver *m_render_driver = nullptr;
    std::unordered_map<uint32_t, TextureId> m_swapchain_textures = {};
    std::array<CommandBuffer *, s_frames_in_flight> m_command_buffers = {};
    std::vector<ComputePass *> m_compute_passes;
    std::vector<RenderPass *> m_render_passes;
    std::vector<Resource> m_deletion_queue;

    uint32_t m_frame_index = 0;

    ResourceOwner<BufferId, Buffer *> m_buffers;
    ResourceOwner<ShaderId, Shader *> m_shaders;
    ResourceOwner<SamplerId, Sampler *> m_samplers;
    ResourceOwner<TextureId, Texture *> m_textures;
    ResourceOwner<PipelineLayoutId, PipelineLayout *> m_pipeline_layouts;
    ResourceOwner<ComputePipelineId, ComputePipeline *> m_compute_pipelines;
    ResourceOwner<RenderPipelineId, RenderPipeline *> m_render_pipelines;
};