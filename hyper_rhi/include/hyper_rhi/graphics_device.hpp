/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/own_ptr.hpp>
#include <hyper_core/ref_ptr.hpp>

#include "hyper_rhi/forward.hpp"
#include "hyper_rhi/resource_handle.hpp"

struct SDL_Window;

namespace hyper_engine
{
    // NOTE: Don't add this to forward.hpp, as it will be only used here
    class DescriptorManager;

    enum class GraphicsApi
    {
        D3D12,
        Vulkan,
    };

    struct GraphicsDeviceDescriptor
    {
        GraphicsApi graphics_api = GraphicsApi::Vulkan;
        bool debug_validation = false;
        bool debug_label = false;
        bool debug_marker = false;
    };

    class GraphicsDevice
    {
    public:
        static constexpr size_t s_frame_count = 2;
        static constexpr size_t s_descriptor_limit = 1000 * 1000;

    public:
        virtual ~GraphicsDevice() = default;

        static OwnPtr<GraphicsDevice> create(const GraphicsDeviceDescriptor &descriptor);

        virtual RefPtr<Surface> create_surface(SDL_Window *window) = 0;
        // FIXME: Make this creatable for every frame for the current command buffer
        virtual RefPtr<CommandList> create_command_list() = 0;

        RefPtr<Buffer> create_buffer(const BufferDescriptor &descriptor);
        RefPtr<Buffer> create_buffer(const BufferDescriptor &descriptor, ResourceHandle handle);

        RefPtr<ComputePipeline> create_compute_pipeline(const ComputePipelineDescriptor &descriptor);
        RefPtr<RenderPipeline> create_render_pipeline(const RenderPipelineDescriptor &descriptor);
        RefPtr<PipelineLayout> create_pipeline_layout(const PipelineLayoutDescriptor &descriptor);
        RefPtr<ShaderModule> create_shader_module(const ShaderModuleDescriptor &descriptor);

        RefPtr<Sampler> create_sampler(const SamplerDescriptor &descriptor);
        RefPtr<Sampler> create_sampler(const SamplerDescriptor &descriptor, ResourceHandle handle);
        RefPtr<Texture> create_texture(const TextureDescriptor &descriptor);
        RefPtr<TextureView> create_texture_view(const TextureViewDescriptor &descriptor);
        RefPtr<TextureView> create_texture_view(const TextureViewDescriptor &descriptor, ResourceHandle handle);

        virtual void begin_frame(RefPtr<Surface> &surface, uint32_t frame_index) = 0;
        virtual void end_frame() const = 0;
        virtual void execute(const RefPtr<CommandList> &command_list) = 0;
        virtual void present(const RefPtr<Surface> &surface) const = 0;

        virtual void wait_for_idle() const = 0;

        virtual GraphicsApi graphics_api() const = 0;
        virtual bool debug_validation() const = 0;
        virtual bool debug_label() const = 0;
        virtual bool debug_marker() const = 0;

    protected:
        virtual RefPtr<Buffer> create_buffer_platform(const BufferDescriptor &descriptor, ResourceHandle handle) = 0;

        virtual RefPtr<ComputePipeline> create_compute_pipeline_platform(const ComputePipelineDescriptor &descriptor) = 0;
        virtual RefPtr<RenderPipeline> create_render_pipeline_platform(const RenderPipelineDescriptor &descriptor) = 0;
        virtual RefPtr<PipelineLayout> create_pipeline_layout_platform(const PipelineLayoutDescriptor &descriptor) = 0;
        virtual RefPtr<ShaderModule> create_shader_module_platform(const ShaderModuleDescriptor &descriptor) = 0;

        virtual RefPtr<Sampler> create_sampler_platform(const SamplerDescriptor &descriptor, ResourceHandle handle) = 0;
        virtual RefPtr<Texture> create_texture_platform(const TextureDescriptor &descriptor) = 0;
        virtual RefPtr<TextureView> create_texture_view_platform(const TextureViewDescriptor &descriptor, ResourceHandle handle) = 0;

    protected:
        virtual DescriptorManager &descriptor_manager() = 0;
    };
} // namespace hyper_engine
