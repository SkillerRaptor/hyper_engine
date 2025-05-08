/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <vector>

#include "systems/render/render_driver_common.hpp"
#include "systems/render/resource_handle.hpp"
#include "systems/window_system.hpp"

struct Buffer
{
    std::optional<std::string> label;
    uint64_t size = 0;
    BitFlags<BufferUsage> usage = BufferUsage::None;
    std::optional<ResourceHandle> handle = std::nullopt;
};

struct Shader
{
    std::optional<std::string> label;
    ShaderType type = ShaderType::None;
    std::string entry;
    std::string path;
};

struct Sampler
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
    std::optional<ResourceHandle> handle = std::nullopt;
};

struct Texture
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
    std::optional<ResourceHandle> handle = std::nullopt;
};

struct PipelineLayout
{
    std::optional<std::string> label;
    uint32_t push_constant_size = 0;
};

struct ComputePipeline
{
    std::optional<std::string> label;
    PipelineLayout *layout = nullptr;
    Shader *shader = nullptr;
};

struct RenderPipeline
{
    std::optional<std::string> label;
    PipelineLayout *layout;
    Shader *vertex_shader;
    Shader *fragment_shader;
    std::vector<ColorAttachmentState> color_attachment_states;
    PrimitiveState primitive_state;
    DepthStencilState depth_stencil_state;
};

struct CommandBuffer
{
    uint32_t generation = 0;
    bool compute_pass_in_progress = false;
    bool render_pass_in_progress = false;
    bool swapchain_texture_acquired = false;
};

struct ComputePass
{
    CommandBuffer *command_buffer = nullptr;
    ComputePipeline *compute_pipeline = nullptr;
    bool has_label = false;
    bool ended = false;
};

struct RenderPassColorAttachment
{
    Texture *texture = nullptr;
    Operations operations = {};
};

struct RenderPassDepthStencilAttachment
{
    Texture *texture = nullptr;
    Operations depth_operations = {};
};

struct RenderPass
{
    CommandBuffer *command_buffer = nullptr;
    RenderPipeline *render_pipeline = nullptr;
    bool has_label = false;
    bool ended = false;
};

class RenderDriver
{
public:
    virtual ~RenderDriver() = default;

    virtual void initialize(WindowSystem &window_system, WindowId window) = 0;
    virtual void shutdown() = 0;

    virtual void wait_idle() const = 0;

    virtual std::vector<Texture *> query_swapchain_textures() = 0;

    // Buffer
    virtual Buffer *
        create_buffer(const std::optional<std::string> &label, uint64_t byte_size, BitFlags<BufferUsage> usage, bool staging) const = 0;
    virtual void destroy_buffer(const Buffer *buffer) const = 0;

    virtual void *map_buffer(const Buffer *buffer) const = 0;
    virtual void unmap_buffer(const Buffer *buffer) const = 0;

    // Shader
    virtual Shader *
        create_shader(const std::optional<std::string> &label, ShaderType type, std::string_view entry, std::string_view path) const = 0;
    virtual void destroy_shader(const Shader *shader) const = 0;

    // Sampler
    virtual Sampler *create_sampler(
        const std::optional<std::string> &label,
        Filter mag_filter,
        Filter min_filter,
        Filter mipmap_filter,
        AddressMode address_mode_u,
        AddressMode address_mode_v,
        AddressMode address_mode_w,
        float mip_lod_bias,
        CompareOperation compare_operation,
        float min_lod,
        float max_lod,
        BorderColor border_color) const = 0;
    virtual void destroy_sampler(const Sampler *sampler) const = 0;

    // Textures
    virtual Texture *create_texture(
        const std::optional<std::string> &label,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint32_t array_size,
        uint32_t mip_levels,
        Format format,
        Dimension dimension,
        BitFlags<TextureUsage> usage) const = 0;
    virtual void destroy_texture(const Texture *texture) const = 0;

    // Pipeline Layout
    virtual PipelineLayout *create_pipeline_layout(const std::optional<std::string> &label, uint32_t push_constant_size) const = 0;
    virtual void destroy_pipeline_layout(const PipelineLayout *pipeline_layout) const = 0;

    // Compute Pipeline
    virtual ComputePipeline *
        create_compute_pipeline(const std::optional<std::string> &label, const PipelineLayout *layout, const Shader *shader) const = 0;
    virtual void destroy_compute_pipeline(const ComputePipeline *compute_pipeline) const = 0;

    // Render Pipeline
    virtual RenderPipeline *create_render_pipeline(
        const std::optional<std::string> &label,
        const PipelineLayout *layout,
        const Shader *vertex_shader,
        const Shader *fragment_shader,
        const std::vector<ColorAttachmentState> &color_attachment_states,
        const PrimitiveState &primitive_state,
        const DepthStencilState &depth_stencil_state) const = 0;
    virtual void destroy_render_pipeline(const RenderPipeline *render_pipeline) const = 0;

    // Command Buffer
    virtual CommandBuffer *create_command_buffer() const = 0;
    virtual void destroy_command_buffer(const CommandBuffer *command_buffer) const = 0;

    virtual void acquire_command_buffer(const CommandBuffer *command_buffer) const = 0;
    virtual void submit_command_buffer(CommandBuffer *command_buffer) const = 0;

    virtual void bind_buffer(const Buffer *buffer) const = 0;
    virtual void bind_sampler(const Sampler *sampler) const = 0;
    virtual void bind_texture(const Texture *texture) const = 0;

    virtual void copy_buffer_to_buffer(
        const CommandBuffer *command_buffer,
        const Buffer *src,
        uint32_t src_offset,
        const Buffer *dst,
        uint32_t dst_offset,
        uint32_t size) const = 0;
    virtual void copy_buffer_to_texture(
        const CommandBuffer *command_buffer,
        const Buffer *src,
        uint32_t src_offset,
        Texture *dst,
        Offset3d dst_offset,
        Extent3d dst_extent,
        uint32_t dst_mip_level,
        uint32_t dst_array_index) const = 0;
    virtual void copy_texture_to_buffer(
        const CommandBuffer *command_buffer,
        Texture *src,
        Offset3d src_offset,
        Extent3d src_extent,
        uint32_t src_mip_level,
        uint32_t src_array_index,
        const Buffer *dst,
        uint32_t dst_offset) const = 0;
    virtual void copy_texture_to_texture(
        const CommandBuffer *command_buffer,
        Texture *src,
        Offset3d src_offset,
        uint32_t src_mip_level,
        uint32_t src_array_index,
        Texture *dst,
        Offset3d dst_offset,
        uint32_t dst_mip_level,
        uint32_t dst_array_index,
        Extent3d extent) const = 0;

    virtual void push_constants(const CommandBuffer *command_buffer, const PipelineLayout *pipeline_layout, const void *data, uint32_t size) = 0;

    virtual std::pair<uint32_t, bool> acquire_swapchain_texture(const CommandBuffer *command_buffer) = 0;
    virtual void present() = 0;

    virtual void begin_gpu_marker(const CommandBuffer *command_buffer, Label label) const = 0;
    virtual void end_gpu_marker(const CommandBuffer *command_buffer) const = 0;

    // Compute Pass
    virtual void begin_compute_pass(const CommandBuffer *command_buffer) const = 0;
    virtual void end_compute_pass(const CommandBuffer *command_buffer) const = 0;

    virtual void bind_compute_pipeline(const CommandBuffer *command_buffer, const ComputePipeline *pipeline) const = 0;

    // Render Pass
    virtual void begin_render_pass(
        const CommandBuffer *command_buffer,
        const std::vector<RenderPassColorAttachment> &color_attachments,
        const std::optional<RenderPassDepthStencilAttachment> &depth_stencil_attachment) const = 0;
    virtual void end_render_pass(const CommandBuffer *command_buffer) const = 0;

    virtual void bind_render_pipeline(const CommandBuffer *command_buffer, const RenderPipeline *pipeline) const = 0;
    virtual void bind_index_buffer(const CommandBuffer *command_buffer, const Buffer *buffer) const = 0;

    virtual void set_viewport(const CommandBuffer *command_buffer, float x, float y, float width, float height, float min_depth, float max_depth)
        const = 0;
    virtual void set_scissor(const CommandBuffer *command_buffer, int32_t x, int32_t y, uint32_t width, uint32_t height) const = 0;
    virtual void
        draw(const CommandBuffer *command_buffer, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
            const = 0;
    virtual void draw_indexed(
        const CommandBuffer *command_buffer,
        uint32_t index_count,
        uint32_t instance_count,
        uint32_t first_index,
        int32_t vertex_offset,
        uint32_t first_instance) const = 0;
};