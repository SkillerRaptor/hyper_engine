/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "hyper_rhi/compute_pipeline.hpp"

namespace he {

class VulkanComputePipeline : public ComputePipeline {
public:
    VulkanComputePipeline(const ComputePipelineDescriptor &, VkDevice);
    ~VulkanComputePipeline() override;

    VkPipeline raw() const { return m_raw; }

private:
    VkDevice m_device = VK_NULL_HANDLE;

    VkPipeline m_raw = VK_NULL_HANDLE;
};

} // namespace he
