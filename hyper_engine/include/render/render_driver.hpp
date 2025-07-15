/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include "core/prerequisites.hpp"
#include "platform/window_events.hpp"
#include "render/definitions.hpp"

struct Buffer
{
    BufferDescriptor desc {};
};

struct Shader
{
    ShaderDescriptor desc {};
};

struct Sampler
{
    SamplerDescriptor desc {};
};

struct Texture
{
    TextureDescriptor desc {};
    std::vector<TextureViewId> views {};
};

struct TextureView
{
    TextureViewDescriptor desc {};
};

struct PipelineLayout
{
    PipelineLayoutDescriptor desc {};
};

struct ComputePipeline
{
    ComputePipelineDescriptor desc {};
};

struct RenderPipeline
{
    RenderPipelineDescriptor desc {};
};

struct CommandBuffer
{
    bool compute_pass_in_progress { false };
    bool render_pass_in_progress { false };
};

enum class GraphicsApi
{
    Vulkan,
};

class RenderDriver
{
private:
    HE_NON_COPYABLE(RenderDriver);
    HE_NON_MOVABLE(RenderDriver);

public:
    static std::unique_ptr<RenderDriver> create(GraphicsApi, void *native_window, u32 width, u32 height);
    RenderDriver() = default;
    virtual ~RenderDriver() = default;

    virtual void wait_idle() const = 0;

    virtual BufferId create_buffer(const BufferDescriptor &) = 0;
    virtual void destroy_buffer(BufferId) = 0;

    virtual void *map_buffer(BufferId) const = 0;
    virtual void unmap_buffer(BufferId) const = 0;

    virtual void bind_buffer_to_slot(BufferId, ResourceHandle) const = 0;

    virtual ShaderId create_shader(const ShaderDescriptor &) = 0;
    virtual void destroy_shader(ShaderId) = 0;

    virtual SamplerId create_sampler(const SamplerDescriptor &) = 0;
    virtual void destroy_sampler(SamplerId) = 0;

    virtual void bind_sampler_to_slot(SamplerId, ResourceHandle) const = 0;

    virtual TextureId create_texture(const TextureDescriptor &) = 0;
    virtual void destroy_texture(TextureId) = 0;

    virtual TextureViewId create_texture_view(const TextureViewDescriptor &) = 0;
    virtual void destroy_texture_view(TextureViewId) = 0;

    virtual void bind_texture_view_to_slot(TextureViewId, ResourceHandle) const = 0;

    virtual PipelineLayoutId create_pipeline_layout(const PipelineLayoutDescriptor &) = 0;
    virtual void destroy_pipeline_layout(PipelineLayoutId) = 0;

    virtual ComputePipelineId create_compute_pipeline(const ComputePipelineDescriptor &) = 0;
    virtual void destroy_compute_pipeline(ComputePipelineId) = 0;

    virtual RenderPipelineId create_render_pipeline(const RenderPipelineDescriptor &) = 0;
    virtual void destroy_render_pipeline(RenderPipelineId) = 0;

    // Command Buffer
    virtual CommandBufferId create_command_buffer() = 0;
    virtual void destroy_command_buffer(CommandBufferId) = 0;

    virtual void acquire_command_buffer(CommandBufferId) = 0;
    virtual void submit_command_buffer(CommandBufferId) = 0;

    virtual std::pair<TextureViewId, bool> acquire_swapchain_texture(CommandBufferId) = 0;
    virtual void present() = 0;
    virtual std::vector<TextureViewId> query_swapchain_texture_views() const = 0;
    virtual void on_resize(const WindowResizeEvent &event) = 0;

    virtual void begin_gpu_marker(CommandBufferId, PassLabel) const = 0;
    virtual void end_gpu_marker(CommandBufferId) const = 0;
    virtual void insert_gpu_marker(CommandBufferId, PassLabel) const = 0;

    virtual void copy_buffer_to_buffer(
        CommandBufferId, BufferTargetDescriptor src, BufferTargetDescriptor dst, usize size) const
        = 0;
    virtual void copy_buffer_to_texture(
        CommandBufferId, BufferTargetDescriptor src, TextureTargetDescriptor dst, Extent3d size) const
        = 0;
    virtual void copy_texture_to_buffer(
        CommandBufferId, TextureTargetDescriptor src, BufferTargetDescriptor dst, Extent3d size) const
        = 0;
    virtual void copy_texture_to_texture(
        CommandBufferId, TextureTargetDescriptor src, TextureTargetDescriptor dst, Extent3d size) const
        = 0;

    virtual void push_constants(CommandBufferId, PipelineLayoutId, const void *data, usize size) = 0;

    // Compute Pass
    virtual void begin_compute_pass(CommandBufferId, const ComputePassDescriptor &) const = 0;
    virtual void end_compute_pass(CommandBufferId) const = 0;

    virtual void bind_compute_pipeline(CommandBufferId, ComputePipelineId) const = 0;

    // Render Pass
    virtual void begin_render_pass(CommandBufferId, const RenderPassDescriptor &) const = 0;
    virtual void end_render_pass(CommandBufferId) const = 0;

    virtual void bind_render_pipeline(CommandBufferId, RenderPipelineId) const = 0;
    virtual void bind_index_buffer(CommandBufferId, BufferId) const = 0;

    virtual void set_viewport(CommandBufferId, f32 x, f32 y, f32 width, f32 height, f32 min_depth, f32 max_depth) const = 0;
    virtual void set_scissor(CommandBufferId, Offset2d, Extent2d) const = 0;
    virtual void draw(CommandBufferId, u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) const = 0;
    virtual void draw_indexed(
        CommandBufferId, u32 index_count, u32 instance_count, u32 first_index, i32 vertex_offset, u32 first_instance) const
        = 0;
};
