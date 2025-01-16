/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

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

        static std::unique_ptr<GraphicsDevice> create(const GraphicsDeviceDescriptor &descriptor);

        virtual std::shared_ptr<Surface> create_surface(SDL_Window *window) = 0;
        // FIXME: Make this creatable for every frame for the current command buffer
        virtual std::shared_ptr<CommandList> create_command_list() = 0;

        std::shared_ptr<Buffer> create_buffer(const BufferDescriptor &descriptor);
        std::shared_ptr<Buffer> create_buffer(const BufferDescriptor &descriptor, ResourceHandle handle);

        std::shared_ptr<ComputePipeline> create_compute_pipeline(const ComputePipelineDescriptor &descriptor);
        std::shared_ptr<RenderPipeline> create_render_pipeline(const RenderPipelineDescriptor &descriptor);
        std::shared_ptr<PipelineLayout> create_pipeline_layout(const PipelineLayoutDescriptor &descriptor);
        std::shared_ptr<ShaderModule> create_shader_module(const ShaderModuleDescriptor &descriptor);

        std::shared_ptr<Sampler> create_sampler(const SamplerDescriptor &descriptor);
        std::shared_ptr<Sampler> create_sampler(const SamplerDescriptor &descriptor, ResourceHandle handle);
        std::shared_ptr<Texture> create_texture(const TextureDescriptor &descriptor);
        std::shared_ptr<TextureView> create_texture_view(const TextureViewDescriptor &descriptor);
        std::shared_ptr<TextureView> create_texture_view(const TextureViewDescriptor &descriptor, ResourceHandle handle);

        virtual void begin_frame(std::shared_ptr<Surface> &surface, uint32_t frame_index) = 0;
        virtual void end_frame() const = 0;
        virtual void execute(const std::shared_ptr<CommandList> &command_list) = 0;
        virtual void present(const std::shared_ptr<Surface> &surface) const = 0;

        virtual void wait_for_idle() const = 0;

        virtual GraphicsApi graphics_api() const = 0;
        virtual bool debug_validation() const = 0;
        virtual bool debug_label() const = 0;
        virtual bool debug_marker() const = 0;

    protected:
        virtual std::shared_ptr<Buffer> create_buffer_platform(const BufferDescriptor &descriptor, ResourceHandle handle) = 0;

        virtual std::shared_ptr<ComputePipeline> create_compute_pipeline_platform(const ComputePipelineDescriptor &descriptor) = 0;
        virtual std::shared_ptr<RenderPipeline> create_render_pipeline_platform(const RenderPipelineDescriptor &descriptor) = 0;
        virtual std::shared_ptr<PipelineLayout> create_pipeline_layout_platform(const PipelineLayoutDescriptor &descriptor) = 0;
        virtual std::shared_ptr<ShaderModule> create_shader_module_platform(const ShaderModuleDescriptor &descriptor) = 0;

        virtual std::shared_ptr<Sampler> create_sampler_platform(const SamplerDescriptor &descriptor, ResourceHandle handle) = 0;
        virtual std::shared_ptr<Texture> create_texture_platform(const TextureDescriptor &descriptor) = 0;
        virtual std::shared_ptr<TextureView> create_texture_view_platform(const TextureViewDescriptor &descriptor, ResourceHandle handle) = 0;

    protected:
        virtual DescriptorManager &descriptor_manager() = 0;
    };
} // namespace hyper_engine
