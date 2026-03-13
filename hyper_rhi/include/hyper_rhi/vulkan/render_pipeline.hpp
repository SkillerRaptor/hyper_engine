/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <hyper_core/prerequisites.hpp>

#include "hyper_rhi/render_pipeline.hpp"

namespace he {

class VulkanGraphicsDevice;

class VulkanRenderPipeline {
public:
    VulkanRenderPipeline(VulkanGraphicsDevice &, const RenderPipelineDescriptor &);
    ~VulkanRenderPipeline();

    HE_ALWAYS_INLINE VkPipeline raw() const { return m_raw; }

private:
    VulkanGraphicsDevice &m_graphics_device;

    VkPipeline m_raw { VK_NULL_HANDLE };
};

} // namespace he
