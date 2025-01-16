/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_rhi/graphics_device.hpp"

#include <hyper_core/ref_ptr.hpp>
#include <hyper_rhi/forward.hpp>
#include <hyper_rhi/shader_module.hpp>
#include <hyper_rhi/render_pipeline.hpp>
#include <hyper_rhi/pipeline_layout.hpp>

namespace hyper_engine
{
    class GridPass
    {
    public:
        GridPass(
            GraphicsDevice &graphics_device,
            const ShaderCompiler &shader_compiler,
            const RefPtr<Texture> &render_texture,
            const RefPtr<TextureView> &render_texture_view,
            const RefPtr<Texture> &depth_texture,
            const RefPtr<TextureView> &depth_texture_view);

        void render(const RefPtr<CommandList> &command_list) const;

    private:
        const RefPtr<Texture> &m_render_texture;
        const RefPtr<TextureView> &m_render_texture_view;
        const RefPtr<Texture> &m_depth_texture;
        const RefPtr<TextureView> &m_depth_texture_view;

        RefPtr<PipelineLayout> m_pipeline_layout;
        RefPtr<ShaderModule> m_vertex_shader;
        RefPtr<ShaderModule> m_fragment_shader;
        // FIXME: This should be a RefPtr
        RefPtr<RenderPipeline> m_pipeline;
    };
} // namespace hyper_engine