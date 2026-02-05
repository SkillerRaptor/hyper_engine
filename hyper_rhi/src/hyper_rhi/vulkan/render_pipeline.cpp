/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/render_pipeline.hpp"

#include "hyper_rhi/validation.hpp"

namespace he
{
    VulkanRenderPipeline::VulkanRenderPipeline(
        VulkanGraphicsDevice &graphics_device, const RenderPipelineDescriptor &desc)
        : RenderPipeline(desc)
        , m_graphics_device(graphics_device)
    {
        validate_render_pipeline_descriptor(desc);
    }

    VulkanRenderPipeline::~VulkanRenderPipeline() { }
} // namespace he
