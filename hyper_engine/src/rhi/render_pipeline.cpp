/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/render_pipeline.hpp"

#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/shader_module.hpp"

namespace hyper_engine
{
    RenderPipeline::RenderPipeline(const RenderPipelineDescriptor &descriptor)
        : m_label(descriptor.label)
        , m_layout(descriptor.layout)
        , m_vertex_shader(descriptor.vertex_shader)
        , m_fragment_shader(descriptor.fragment_shader)
        , m_color_attachment_states(descriptor.color_attachment_states)
        , m_primitive_state(descriptor.primitive_state)
        , m_depth_stencil_state(descriptor.depth_stencil_state)
    {
    }

    std::string_view RenderPipeline::label() const
    {
        return m_label;
    }

    std::shared_ptr<PipelineLayout> RenderPipeline::layout() const
    {
        return m_layout;
    }

    std::shared_ptr<ShaderModule> RenderPipeline::vertex_shader() const
    {
        return m_vertex_shader;
    }

    std::shared_ptr<ShaderModule> RenderPipeline::fragment_shader() const
    {
        return m_fragment_shader;
    }

    const std::vector<ColorAttachmentState> &RenderPipeline::color_attachment_states() const
    {
        return m_color_attachment_states;
    }

    PrimitiveState RenderPipeline::primitive_state() const
    {
        return m_primitive_state;
    }

    DepthStencilState RenderPipeline::depth_stencil_state() const
    {
        return m_depth_stencil_state;
    }
} // namespace hyper_engine