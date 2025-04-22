/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "systems/render_system.hpp"

class RenderDriver
{
public:
    virtual ~RenderDriver() = default;

    virtual void initialize(const WindowSystem &window_system, WindowSystem::WindowId window) = 0;
    virtual std::vector<RS::Texture *> query_swapchain_textures() = 0;

    // Buffer
    virtual RS::Buffer *create_buffer(const std::optional<std::string> &label, uint64_t byte_size, BitFlags<RS::BufferUsage> usage) = 0;
    virtual RS::Buffer *create_staging_buffer(const std::optional<std::string> &label, uint64_t byte_size, BitFlags<RS::BufferUsage> usage) = 0;
    virtual void destroy_buffer(RS::Buffer *buffer) = 0;

    // Shader
    virtual RS::Shader *
        create_shader(const std::optional<std::string> &label, RS::ShaderType type, const std::string &entry, const std::string &path) = 0;
    virtual void destroy_shader(RS::Shader *shader) = 0;

    // Sampler
    virtual RS::Sampler *create_sampler(
        const std::optional<std::string> &label,
        RS::Filter mag_filter,
        RS::Filter min_filter,
        RS::Filter mipmap_filter,
        RS::AddressMode address_mode_u,
        RS::AddressMode address_mode_v,
        RS::AddressMode address_mode_w,
        float mip_lod_bias,
        RS::CompareOperation compare_operation,
        float min_lod,
        float max_lod,
        RS::BorderColor border_color) = 0;
    virtual void destroy_sampler(RS::Sampler *sampler) = 0;

    // Textures
    virtual RS::Texture *create_texture(
        const std::optional<std::string> &label,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint32_t array_size,
        uint32_t mip_levels,
        RS::Format format,
        RS::Dimension dimension,
        BitFlags<RS::TextureUsage> usage) = 0;
    virtual void destroy_texture(RS::Texture *texture) = 0;

    // Pipeline Layout
    virtual RS::PipelineLayout *create_pipeline_layout(const std::optional<std::string> &label, uint32_t push_constant_size) = 0;
    virtual void destroy_pipeline_layout(RS::PipelineLayout *pipeline_layout) = 0;

    // Compute Pipeline
    virtual RS::ComputePipeline *
        create_compute_pipeline(const std::optional<std::string> &label, RS::PipelineLayout *layout, RS::Shader *shader) = 0;
    virtual void destroy_compute_pipeline(RS::ComputePipeline *compute_pipeline) = 0;

    // Render Pipeline
    virtual RS::RenderPipeline *create_render_pipeline(
        const std::optional<std::string> &label,
        RS::PipelineLayout *layout,
        RS::Shader *vertex_shader,
        RS::Shader *fragment_shader,
        const std::vector<RS::ColorAttachmentState> &color_attachment_states,
        const RS::PrimitiveState &primitive_state,
        const RS::DepthStencilState &depth_stencil_state) = 0;
    virtual void destroy_render_pipeline(RS::RenderPipeline *render_pipeline) = 0;

    // Command Buffer
    virtual RS::CommandBuffer *create_command_buffer() = 0;
    virtual void destroy_command_buffer(RS::CommandBuffer *command_buffer) = 0;

    virtual void acquire_command_buffer(RS::CommandBuffer *command_buffer) = 0;
    virtual void submit_command_buffer(RS::CommandBuffer *command_buffer) = 0;

    virtual uint32_t acquire_swapchain_texture(RS::CommandBuffer *command_buffer) = 0;
    virtual void present() = 0;

    // Compute Pass
    virtual void begin_compute_pass(RS::CommandBuffer *command_buffer) = 0;
    virtual void end_compute_pass(RS::CommandBuffer *command_buffer) = 0;

    virtual void bind_compute_pipeline(RS::CommandBuffer *command_buffer, RS::ComputePipeline *pipeline) = 0;

    // Render Pass
    virtual void begin_render_pass(RS::CommandBuffer *command_buffer, const RS::RenderPassDescriptor &descriptor, RS::Texture *texture) = 0;
    virtual void end_render_pass(RS::CommandBuffer *command_buffer) = 0;

    virtual void bind_render_pipeline(RS::CommandBuffer *command_buffer, RS::RenderPipeline *pipeline) = 0;

    virtual void set_viewport(RS::CommandBuffer *command_buffer, const RS::Viewport &viewport) = 0;
    virtual void set_scissor(RS::CommandBuffer *command_buffer, const RS::Scissor &scissor) = 0;
    virtual void draw(
        RS::CommandBuffer *command_buffer,
        uint32_t vertex_count,
        uint32_t instance_count,
        uint32_t first_vertex,
        uint32_t first_instance) = 0;
};