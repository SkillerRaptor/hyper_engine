/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "rhi/vulkan/render_pipeline.hpp"

#include <hyper_core/logger.hpp>

#include "rhi/validation.hpp"

namespace he
{
    RefPtr<VulkanRenderPipeline>
        VulkanRenderPipeline::create(VulkanGraphicsDevice &graphics_device, const RenderPipelineDescriptor &desc)
    {
        RefPtr<VulkanRenderPipeline> render_pipeline
            = wrap_ref<VulkanRenderPipeline>(new VulkanRenderPipeline(graphics_device, desc));
        if (!render_pipeline->initialize(desc))
        {
            HE_ERROR("Failed to initialize render pipeline");
            return nullptr;
        }

        return render_pipeline;
    }

    VulkanRenderPipeline::~VulkanRenderPipeline() { }

    bool VulkanRenderPipeline::initialize(const RenderPipelineDescriptor &desc)
    {
        if (!validate_render_pipeline_descriptor(desc))
        {
            return false;
        }

        return true;
    }
} // namespace he
