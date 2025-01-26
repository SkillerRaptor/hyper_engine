/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/render_passes/opaque_pass.hpp"

#include <hyper_rhi/buffer.hpp>
#include <hyper_rhi/command_list.hpp>
#include <hyper_rhi/render_pass.hpp>
#include <hyper_rhi/texture_view.hpp>

#include "hyper_rendering/renderer.hpp"

#include "shader_interop.h"

namespace hyper_engine
{
    OpaquePass::OpaquePass(
        const std::shared_ptr<TextureView> &render_texture_view,
        const std::shared_ptr<TextureView> &depth_texture_view,
        const std::shared_ptr<Buffer> &scene_buffer)
        : m_render_texture_view(render_texture_view)
        , m_depth_texture_view(depth_texture_view)
        , m_scene_buffer(scene_buffer)
    {
    }

    void OpaquePass::render(const std::shared_ptr<CommandList> &command_list, const DrawContext &draw_context) const
    {
        const std::shared_ptr<RenderPass> render_pass = command_list->begin_render_pass({
            .label = "Opaque",
            .label_color =
                {
                    .red = 254,
                    .green = 17,
                    .blue = 85,
                },
            .color_attachments =
                {
                    {
                        .view = m_render_texture_view,
                        .operation =
                            {
                                .load_operation = LoadOperation::Clear,
                                .store_operation = StoreOperation::Store,
                            },
                    },
                },
            .depth_stencil_attachment =
                {
                    .view = m_depth_texture_view,
                    .depth_operation =
                        {
                            .load_operation = LoadOperation::Clear,
                            .store_operation = StoreOperation::Store,
                        },
                },
        });

        for (const RenderObject &render_object : draw_context.opaque_surfaces)
        {
            render_pass->set_pipeline(render_object.material->pipeline);

            render_pass->set_index_buffer(render_object.index_buffer);

            const ObjectPushConstants mesh_push_constants = {
                .scene = m_scene_buffer->handle(),
                .mesh = render_object.mesh_buffer->handle(),
                .material = render_object.material->buffer->handle(),
                .padding_0 = 0,
                .transform_matrix = render_object.transform,
            };
            render_pass->set_push_constants(&mesh_push_constants, sizeof(ObjectPushConstants));

            render_pass->draw_indexed(render_object.index_count, 1, render_object.first_index, 0, 0);
        }

        for (const RenderObject &render_object : draw_context.transparent_surfaces)
        {
            render_pass->set_pipeline(render_object.material->pipeline);

            render_pass->set_index_buffer(render_object.index_buffer);

            const ObjectPushConstants mesh_push_constants = {
                .scene = m_scene_buffer->handle(),
                .mesh = render_object.mesh_buffer->handle(),
                .material = render_object.material->buffer->handle(),
                .padding_0 = 0,
                .transform_matrix = render_object.transform,
            };
            render_pass->set_push_constants(&mesh_push_constants, sizeof(ObjectPushConstants));

            render_pass->draw_indexed(render_object.index_count, 1, render_object.first_index, 0, 0);
        }
    }
} // namespace hyper_engine