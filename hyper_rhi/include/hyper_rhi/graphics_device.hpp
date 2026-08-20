/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include <hyper_core/types.hpp>
#include <hyper_platform/forward.hpp>

#include "hyper_rhi/forward.hpp"

namespace he {

enum class GraphicsApi {
    D3D12,
    Vulkan,
};

class GraphicsDevice {
protected:
    static constexpr u32 s_frames_in_flight = 3;

public:
    static std::unique_ptr<GraphicsDevice> create(GraphicsApi, const Window &);
    virtual ~GraphicsDevice() = default;

    Buffer *create_buffer(const BufferDescriptor &);
    void destroy_buffer(Buffer *);

    Shader *create_shader(const ShaderDescriptor &);
    void destroy_shader(Shader *);

    Sampler *create_sampler(const SamplerDescriptor &);
    void destroy_sampler(Sampler *);

    Texture *create_texture(const TextureDescriptor &);
    void destroy_texture(Texture *);

    TextureView *create_texture_view(const TextureViewDescriptor &);
    void destroy_texture_view(TextureView *);

    PipelineLayout *create_pipeline_layout(const PipelineLayoutDescriptor &);
    void destroy_pipeline_layout(PipelineLayout *);

    ComputePipeline *create_compute_pipeline(const ComputePipelineDescriptor &);
    void destroy_compute_pipeline(ComputePipeline *);

    RenderPipeline *create_render_pipeline(const RenderPipelineDescriptor &);
    void destroy_render_pipeline(RenderPipeline *);

protected:
    virtual Buffer *create_buffer_impl(const BufferDescriptor &) = 0;
    virtual void destroy_buffer_impl(Buffer *) = 0;

    virtual Shader *create_shader_impl(const ShaderDescriptor &) = 0;
    virtual void destroy_shader_impl(Shader *) = 0;

    virtual Sampler *create_sampler_impl(const SamplerDescriptor &) = 0;
    virtual void destroy_sampler_impl(Sampler *) = 0;

    virtual Texture *create_texture_impl(const TextureDescriptor &) = 0;
    virtual void destroy_texture_impl(Texture *) = 0;

    virtual TextureView *create_texture_view_impl(const TextureViewDescriptor &) = 0;
    virtual void destroy_texture_view_impl(TextureView *) = 0;

    virtual PipelineLayout *create_pipeline_layout_impl(const PipelineLayoutDescriptor &) = 0;
    virtual void destroy_pipeline_layout_impl(PipelineLayout *) = 0;

    virtual ComputePipeline *create_compute_pipeline_impl(const ComputePipelineDescriptor &) = 0;
    virtual void destroy_compute_pipeline_impl(ComputePipeline *) = 0;

    virtual RenderPipeline *create_render_pipeline_impl(const RenderPipelineDescriptor &) = 0;
    virtual void destroy_render_pipeline_impl(RenderPipeline *) = 0;

private:
    static void validate_buffer_descriptor(const BufferDescriptor &);
    static void validate_shader_descriptor(const ShaderDescriptor &);
    static void validate_sampler_descriptor(const SamplerDescriptor &);
    static void validate_texture_descriptor(const TextureDescriptor &);
    static void validate_texture_view_descriptor(const TextureViewDescriptor &);
    static void validate_pipeline_layout_descriptor(const PipelineLayoutDescriptor &);
    static void validate_compute_pipeline_descriptor(const ComputePipelineDescriptor &);
    static void validate_render_pipeline_descriptor(const RenderPipelineDescriptor &);
};

} // namespace he
