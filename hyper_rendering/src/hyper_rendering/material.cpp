/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/material.hpp"

#include <hyper_core/assertion.hpp>
#include <hyper_core/filesystem.hpp>
#include <hyper_rhi/buffer.hpp>
#include <hyper_rhi/command_list.hpp>
#include <hyper_rhi/graphics_device.hpp>
#include <hyper_rhi/pipeline_layout.hpp>
#include <hyper_rhi/render_pipeline.hpp>
#include <hyper_rhi/sampler.hpp>
#include <hyper_rhi/shader_compiler.hpp>
#include <hyper_rhi/shader_module.hpp>
#include <hyper_rhi/texture.hpp>
#include <hyper_rhi/texture_view.hpp>

#include "shader_interop.h"

namespace hyper_engine
{
    GltfMetallicRoughness::GltfMetallicRoughness(
        GraphicsDevice &graphics_device,
        const ShaderCompiler &shader_compiler,
        const RefPtr<Texture> &render_texture,
        const RefPtr<Texture> &depth_texture)
        : m_graphics_device(graphics_device)
    {
        const RefPtr<ShaderModule> vertex_shader = m_graphics_device.create_shader_module({
            .label = "Mesh",
            .type = ShaderType::Vertex,
            .entry_name = "vs_main",
            .bytes = shader_compiler
                         .compile({
                             .type = ShaderType::Vertex,
                             .entry_name = "vs_main",
                             .data = filesystem::read_file("./assets/shaders/mesh_shader.hlsl"),
                         })
                         .spirv,
        });

        const RefPtr<ShaderModule> fragment_shader = m_graphics_device.create_shader_module({
            .label = "Mesh",
            .type = ShaderType::Fragment,
            .entry_name = "fs_main",
            .bytes = shader_compiler
                         .compile({
                             .type = ShaderType::Fragment,
                             .entry_name = "fs_main",
                             .data = filesystem::read_file("./assets/shaders/mesh_shader.hlsl"),
                         })
                         .spirv,
        });

        const RefPtr<PipelineLayout> pipeline_layout = m_graphics_device.create_pipeline_layout({
            .label = "Mesh",
            .push_constant_size = sizeof(ObjectPushConstants),
        });

        m_opaque_pipeline = m_graphics_device.create_render_pipeline({
            .label = "Opaque",
            .layout = pipeline_layout,
            .vertex_shader = vertex_shader,
            .fragment_shader = fragment_shader,
            .color_attachment_states =
                {
                    {
                        .format = render_texture->format(),
                        .blend_state =
                            {
                                .blend_enable = false,
                                .src_blend_factor = BlendFactor::One,
                                .dst_blend_factor = BlendFactor::Zero,
                                .operation = BlendOperation::Add,
                                .alpha_src_blend_factor = BlendFactor::One,
                                .alpha_dst_blend_factor = BlendFactor::Zero,
                                .alpha_operation = BlendOperation::Add,
                                .color_writes = ColorWrites::All,
                            },
                    },
                },
            .primitive_state =
                {
                    .topology = PrimitiveTopology::TriangleList,
                    .front_face = FrontFace::CounterClockwise,
                    .cull_mode = Face::Back,
                    .polygon_mode = PolygonMode::Fill,
                },
            .depth_stencil_state =
                {
                    .depth_test_enable = true,
                    .depth_write_enable = true,
                    .depth_format = depth_texture->format(),
                    .depth_compare_operation = CompareOperation::Less,
                    .depth_bias_state = {},
                },
        });

        m_transparent_pipeline = m_graphics_device.create_render_pipeline({
            .label = "Transparent",
            .layout = pipeline_layout,
            .vertex_shader = vertex_shader,
            .fragment_shader = fragment_shader,
            .color_attachment_states =
                {
                    {
                        .format = render_texture->format(),
                        .blend_state =
                            {
                                .blend_enable = true,
                                .src_blend_factor = BlendFactor::SrcAlpha,
                                .dst_blend_factor = BlendFactor::OneMinusSrcAlpha,
                                .operation = BlendOperation::Add,
                                .alpha_src_blend_factor = BlendFactor::One,
                                .alpha_dst_blend_factor = BlendFactor::Zero,
                                .alpha_operation = BlendOperation::Add,
                                .color_writes = ColorWrites::All,
                            },
                    },
                },
            .primitive_state =
                {
                    .topology = PrimitiveTopology::TriangleList,
                    .front_face = FrontFace::CounterClockwise,
                    .cull_mode = Face::Back,
                    .polygon_mode = PolygonMode::Fill,
                },
            .depth_stencil_state =
                {
                    .depth_test_enable = true,
                    .depth_write_enable = true,
                    // FIXME: Add 2nd pass for transparent stuff
                    .depth_format = depth_texture->format(),
                    .depth_compare_operation = CompareOperation::Less,
                    .depth_bias_state = {},
                },
        });
    }

    MaterialInstance GltfMetallicRoughness::write_material(
        const RefPtr<CommandList> &command_list,
        const MaterialPassType pass_type,
        const MaterialResources &resources) const
    {
        const RefPtr<Buffer> buffer = m_graphics_device.create_buffer({
            .label = "Material",
            .byte_size = sizeof(ShaderMaterial),
            .usage = {BufferUsage::Storage, BufferUsage::ShaderResource},
        });

        const ShaderMaterial shader_material = {
            .color_factors = resources.color_factors,
            .color_texture = resources.color_texture_view->handle(),
            .color_sampler = resources.color_sampler->handle(),
            .padding_0 = 0,
            .padding_1 = 0,
            .metal_roughness_factors = resources.metal_roughness_factors,
            .metal_roughness_texture = resources.metal_roughness_texture_view->handle(),
            .metal_roughness_sampler = resources.metal_roughness_sampler->handle(),
            .padding_2 = 0,
            .padding_3 = 0,
        };

        command_list->write_buffer(buffer, &shader_material, sizeof(ShaderMaterial), 0);

        const RefPtr<RenderPipeline> pipeline = [&]()
        {
            switch (pass_type)
            {
            case MaterialPassType::MainColor:
                return m_opaque_pipeline;
            case MaterialPassType::Transparent:
                return m_transparent_pipeline;
            default:
                HE_UNREACHABLE();
            }
        }();

        const MaterialInstance material_instance = {
            .pipeline = pipeline,
            .pass_type = pass_type,
            .buffer = buffer,
        };

        return material_instance;
    }
} // namespace hyper_engine