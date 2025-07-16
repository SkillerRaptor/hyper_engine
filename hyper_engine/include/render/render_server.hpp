/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <memory>
#include <stack>
#include <unordered_set>

#include "core/prerequisites.hpp"
#include "platform/event_server.hpp"
#include "render/definitions.hpp"
#include "render/resource_handle.hpp"

HE_DEFINE_ID(ComputePass);
HE_DEFINE_ID(RenderPass);

class WindowResizeEvent;
class RenderDriver;

class RenderServer
{
private:
    HE_NON_COPYABLE(RenderServer);
    HE_NON_MOVABLE(RenderServer);

private:
    static constexpr u8 s_frames_in_flight = 3;

    enum class ResourceTag
    {
        None,
        Buffer,
        Shader,
        Sampler,
        Texture,
        TextureView,
        PipelineLayout,
        ComputePipeline,
        RenderPipeline,
    };

    struct Resource
    {
        ResourceTag tag { ResourceTag::None };
        union
        {
            BufferId buffer_id;
            ShaderId shader_id;
            SamplerId sampler_id;
            TextureId texture_id;
            TextureViewId texture_view_id;
            PipelineLayoutId pipeline_layout_id;
            ComputePipelineId compute_pipeline_id;
            RenderPipelineId render_pipeline_id;
        };
        u8 generation { 0 };
    };

    struct ComputePass
    {
        CommandBufferId command_buffer {};
        ComputePipelineId compute_pipeline {};
        bool has_label { false };
        bool ended { false };
    };

    struct RenderPass
    {
        CommandBufferId command_buffer {};
        RenderPipelineId render_pipeline {};
        bool has_label { false };
        bool ended { false };
    };

public:
    static std::unique_ptr<RenderServer> create(EventServer &, void *native_window, u32 width, u32 height);
    ~RenderServer();

    void wait_idle() const;

    BufferId create_buffer(const BufferDescriptor &);
    void destroy_buffer(BufferId);

    ResourceHandle get_buffer_handle(BufferId) const;

    ShaderId create_shader(const ShaderDescriptor &);
    void destroy_shader(ShaderId);

    SamplerId create_sampler(const SamplerDescriptor &);
    void destroy_sampler(SamplerId);

    ResourceHandle get_sampler_handle(SamplerId) const;

    TextureId create_texture(const TextureDescriptor &);
    void destroy_texture(TextureId);

    void generate_mip_maps(CommandBufferId, TextureId);

    TextureViewId create_texture_view(const TextureViewDescriptor &);
    void destroy_texture_view(TextureViewId);

    ResourceHandle get_texture_view_handle(TextureViewId) const;

    PipelineLayoutId create_pipeline_layout(const PipelineLayoutDescriptor &);
    void destroy_pipeline_layout(PipelineLayoutId);

    ComputePipelineId create_compute_pipeline(const ComputePipelineDescriptor &);
    void destroy_compute_pipeline(ComputePipelineId);

    RenderPipelineId create_render_pipeline(const RenderPipelineDescriptor &);
    void destroy_render_pipeline(RenderPipelineId);

    // Commands
    CommandBufferId acquire_command_buffer();
    void submit_command_buffer(CommandBufferId);

    void write_buffer(CommandBufferId, const BufferTargetDescriptor &, const void *data, usize size);

    template <typename T>
    void write_buffer(const CommandBufferId id, const BufferTargetDescriptor &buffer_target, const T &data)
    {
        write_buffer(id, buffer_target, &data, sizeof(T));
    }

    void write_texture(CommandBufferId, const TextureTargetDescriptor &, const void *data, usize size, Extent3d extent);

    template <typename T>
    void write_texture(
        const CommandBufferId id, const TextureTargetDescriptor &texture_target, const T &data, const Extent3d extent)
    {
        write_texture(id, texture_target, &data, sizeof(T), extent);
    }

    void copy_buffer_to_buffer(
        CommandBufferId, const BufferTargetDescriptor &src, const BufferTargetDescriptor &dst, usize size) const;
    void copy_buffer_to_texture(
        CommandBufferId, const BufferTargetDescriptor &src, const TextureTargetDescriptor &dst, Extent3d extent) const;
    void copy_texture_to_buffer(
        CommandBufferId, const TextureTargetDescriptor &src, const BufferTargetDescriptor &dst, Extent3d extent) const;
    void copy_texture_to_texture(
        CommandBufferId, const TextureTargetDescriptor &src, const TextureTargetDescriptor &dst, Extent3d extent) const;

    TextureViewId acquire_swapchain_texture(CommandBufferId);

    // Compute Pass
    ComputePassId begin_compute_pass(CommandBufferId, const ComputePassDescriptor &);
    void end_compute_pass(ComputePassId) const;

    void bind_pipeline(ComputePassId, ComputePipelineId) const;
    void push_constants(ComputePassId, const void *data, usize size) const;

    template <typename T>
    void push_constants(const ComputePassId id, const T &data) const
    {
        push_constants(id, &data, sizeof(T));
    }

    void dispatch(ComputePassId, u32 x, u32 y, u32 z) const;

    // Render Pass
    RenderPassId begin_render_pass(CommandBufferId, const RenderPassDescriptor &);
    void end_render_pass(RenderPassId) const;

    void bind_pipeline(RenderPassId, RenderPipelineId) const;
    void bind_index_buffer(RenderPassId, BufferId) const;
    void push_constants(RenderPassId, const void *data, usize size) const;

    template <typename T>
    void push_constants(const RenderPassId id, const T &data) const
    {
        push_constants(id, &data, sizeof(T));
    }

    void set_viewport(RenderPassId, f32 x, f32 y, f32 width, f32 height, f32 min_depth, f32 max_depth) const;
    void set_scissor(RenderPassId, Offset2d, Extent2d) const;
    void draw(RenderPassId, u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) const;
    void draw_indexed(
        RenderPassId, u32 index_count, u32 instance_count, u32 first_index, i32 vertex_offset, u32 first_instance) const;

private:
    RenderServer(EventServer &,
        std::unique_ptr<RenderDriver>,
        std::array<CommandBufferId, s_frames_in_flight>,
        std::unordered_set<TextureViewId> texture_views);

    ResourceHandle allocate_handle();
    void retire_handle(ResourceHandle);

    void on_resize(const WindowResizeEvent &) const;

private:
    std::unique_ptr<RenderDriver> m_render_driver;
    std::array<CommandBufferId, s_frames_in_flight> m_command_buffers {};
    std::vector<ComputePassId> m_compute_passes {};
    std::vector<RenderPassId> m_render_passes {};
    std::unordered_set<TextureViewId> m_swapchain_texture_views {};

    std::unordered_set<BufferId> m_buffers {};
    std::unordered_set<ShaderId> m_shaders {};
    std::unordered_set<SamplerId> m_samplers {};
    std::unordered_set<TextureId> m_textures {};
    std::unordered_set<TextureViewId> m_texture_views {};
    std::unordered_set<PipelineLayoutId> m_pipeline_layouts {};
    std::unordered_set<ComputePipelineId> m_compute_pipelines {};
    std::unordered_set<RenderPipelineId> m_render_pipelines {};

    std::vector<Resource> m_deletion_queue {};
    std::stack<ResourceHandle> m_recycled_descriptors {};
    u32 m_current_descriptor_index { 0 };

    u32 m_frame_index { 0 };
};
