/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "hyper_rhi/render_pipeline.hpp"

namespace he {

class VulkanRenderPipeline : public RenderPipeline {
public:
    VulkanRenderPipeline(const RenderPipelineDescriptor &, VkDevice);
    ~VulkanRenderPipeline() override;

    VkPipeline raw() const { return m_raw; }

private:
    VkDevice m_device = VK_NULL_HANDLE;

    VkPipeline m_raw = VK_NULL_HANDLE;
};

} // namespace he
