/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_rhi/graphics_device.hpp>
#include <hyper_rhi/surface.hpp>

namespace hyper_render
{
    struct RendererDescriptor
    {
        hyper_rhi::GraphicsDeviceHandle graphics_device;
        hyper_rhi::SurfaceHandle surface;
    };

    class Renderer
    {
    public:
        explicit Renderer(const RendererDescriptor &descriptor);

    private:
        hyper_rhi::GraphicsDeviceHandle m_graphics_device;
        hyper_rhi::SurfaceHandle m_surface;
        hyper_rhi::CommandListHandle m_command_list;
        hyper_rhi::PipelineLayoutHandle m_pipeline_layout;
        hyper_rhi::ShaderModuleHandle m_vertex_shader;
        hyper_rhi::ShaderModuleHandle m_fragment_shader;
        hyper_rhi::GraphicsPipelineHandle m_pipeline;
        hyper_rhi::BufferHandle m_material_buffer;
        hyper_rhi::BufferHandle m_positions_buffer;
        hyper_rhi::BufferHandle m_normals_buffer;
        hyper_rhi::BufferHandle m_mesh_buffer;
        hyper_rhi::BufferHandle m_indices_buffer;
    };
} // namespace hyper_render