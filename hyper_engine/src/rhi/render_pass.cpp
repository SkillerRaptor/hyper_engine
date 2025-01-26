/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/render_pass.hpp"

#include "hyper_rhi/texture_view.hpp"

namespace hyper_engine
{
    RenderPass::RenderPass(const RenderPassDescriptor &descriptor)
        : m_label(descriptor.label)
        , m_label_color(descriptor.label_color)
        , m_color_attachments(descriptor.color_attachments)
        , m_depth_stencil_attachment(descriptor.depth_stencil_attachment)
    {
    }

    std::string_view RenderPass::label() const
    {
        return m_label;
    }

    LabelColor RenderPass::label_color() const
    {
        return m_label_color;
    }

    const std::vector<ColorAttachment> &RenderPass::color_attachments() const
    {
        return m_color_attachments;
    }

    DepthStencilAttachment RenderPass::depth_stencil_attachment() const
    {
        return m_depth_stencil_attachment;
    }
} // namespace hyper_engine