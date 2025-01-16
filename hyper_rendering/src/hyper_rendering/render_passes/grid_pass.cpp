/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/render_passes/grid_pass.hpp"

#include <hyper_core/filesystem.hpp>
#include <hyper_rhi/command_list.hpp>
#include <hyper_rhi/graphics_device.hpp>
#include <hyper_rhi/pipeline_layout.hpp>
#include <hyper_rhi/render_pass.hpp>
#include <hyper_rhi/render_pipeline.hpp>
#include <hyper_rhi/sampler.hpp>
#include <hyper_rhi/shader_compiler.hpp>
#include <hyper_rhi/shader_module.hpp>
#include <hyper_rhi/texture.hpp>
#include <hyper_rhi/texture_view.hpp>

namespace hyper_engine
{
    GridPass::GridPass(
        GraphicsDevice &graphics_device,
        const ShaderCompiler &shader_compiler,
        const RefPtr<Texture> &render_texture,
        const RefPtr<TextureView> &render_texture_view,
        const RefPtr<Texture> &depth_texture,
        const RefPtr<TextureView> &depth_texture_view)
        : m_render_texture(render_texture)
        , m_render_texture_view(render_texture_view)
        , m_depth_texture(depth_texture)
        , m_depth_texture_view(depth_texture_view)
        , m_pipeline_layout(graphics_device.create_pipeline_layout({
              .label = "Grid",
              .push_constant_size = 0,
          }))
        , m_vertex_shader(graphics_device.create_shader_module({
              .label = "Grid",
              .type = ShaderType::Vertex,
              .entry_name = "vs_main",
              .bytes = shader_compiler
                           .compile({
                               .type = ShaderType::Vertex,
                               .entry_name = "vs_main",
                               .data = filesystem::read_file("./assets/shaders/grid_shader.hlsl"),
                           })
                           .spirv,
          }))
        , m_fragment_shader(graphics_device.create_shader_module({
              .label = "Grid",
              .type = ShaderType::Fragment,
              .entry_name = "fs_main",
              .bytes = shader_compiler
                           .compile({
                               .type = ShaderType::Fragment,
                               .entry_name = "fs_main",
                               .data = filesystem::read_file("./assets/shaders/grid_shader.hlsl"),
                           })
                           .spirv,
          }))
        , m_pipeline(graphics_device.create_render_pipeline({
              .label = "Grid",
              .layout = m_pipeline_layout,
              .vertex_shader = m_vertex_shader,
              .fragment_shader = m_fragment_shader,
              .color_attachment_states =
                  {
                      {
                          .format = m_render_texture->format(),
                          .blend_state =
                              {
                                  .blend_enable = true,
                                  .src_blend_factor = BlendFactor::SrcAlpha,
                                  .dst_blend_factor = BlendFactor::One,
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
                      .cull_mode = Face::None,
                      .polygon_mode = PolygonMode::Fill,
                  },
              .depth_stencil_state =
                  {
                      .depth_test_enable = true,
                      .depth_write_enable = true,
                      .depth_format = m_depth_texture->format(),
                      .depth_compare_operation = CompareOperation::Less,
                      .depth_bias_state = {},
                  },
          }))
    {
    }

    void GridPass::render(const RefPtr<CommandList> &command_list) const
    {
        const RefPtr<RenderPass> render_pass = command_list->begin_render_pass({
            .label = "Grid",
            .label_color =
                LabelColor{
                    .red = 51,
                    .green = 187,
                    .blue = 255,
                },
            .color_attachments =
                {
                    {
                        .view = m_render_texture_view,
                        .operation =
                            {
                                .load_operation = LoadOperation::Load,
                                .store_operation = StoreOperation::Store,
                            },
                    },
                },
            .depth_stencil_attachment =
                {
                    .view = m_depth_texture_view,
                    .depth_operation =
                        {
                            .load_operation = LoadOperation::Load,
                            .store_operation = StoreOperation::Store,
                        },
                },
        });

        render_pass->set_pipeline(m_pipeline);

        render_pass->draw(6, 1, 0, 0);
    }
} // namespace hyper_engine