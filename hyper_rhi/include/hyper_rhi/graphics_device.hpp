/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include "hyper_rhi/buffer.hpp"
#include "hyper_rhi/command_list.hpp"
#include "hyper_rhi/compute_pipeline.hpp"
#include "hyper_rhi/graphics_pipeline.hpp"
#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/shader_module.hpp"
#include "hyper_rhi/surface.hpp"
#include "hyper_rhi/texture.hpp"

namespace hyper_rhi
{
    enum class GraphicsApi
    {
        D3D12,
        Vulkan,
    };

    struct GraphicsDeviceDescriptor
    {
        GraphicsApi graphics_api = GraphicsApi::Vulkan;
        bool debug_mode = false;
    };

    class GraphicsDevice
    {
    public:
        static std::shared_ptr<GraphicsDevice> create(const GraphicsDeviceDescriptor &descriptor);
        virtual ~GraphicsDevice() = default;

        [[nodiscard]] virtual SurfaceHandle create_surface(const SurfaceDescriptor &descriptor) = 0;

        [[nodiscard]] virtual BufferHandle create_buffer(const BufferDescriptor &descriptor) = 0;
        [[nodiscard]] virtual CommandListHandle create_command_list() = 0;
        [[nodiscard]] virtual ComputePipelineHandle create_compute_pipeline(const ComputePipelineDescriptor &descriptor) = 0;
        [[nodiscard]] virtual GraphicsPipelineHandle create_graphics_pipeline(const GraphicsPipelineDescriptor &descriptor) = 0;
        [[nodiscard]] virtual PipelineLayoutHandle create_pipeline_layout(const PipelineLayoutDescriptor &descriptor) = 0;
        [[nodiscard]] virtual ShaderModuleHandle create_shader_module(const ShaderModuleDescriptor &descriptor) = 0;
        [[nodiscard]] virtual TextureHandle create_texture(const TextureDescriptor &descriptor) = 0;
    };

    using GraphicsDeviceHandle = std::shared_ptr<GraphicsDevice>;
} // namespace hyper_rhi
