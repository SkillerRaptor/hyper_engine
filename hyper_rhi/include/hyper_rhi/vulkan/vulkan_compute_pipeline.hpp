/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_rhi/compute_pipeline.hpp"
#include "hyper_rhi/vulkan/vulkan_common.hpp"

namespace hyper_engine
{
    class VulkanGraphicsDevice;

    class VulkanComputePipeline final : public ComputePipeline
    {
    public:
        VulkanComputePipeline(const ComputePipelineDescriptor &descriptor, VulkanGraphicsDevice &graphics_device, VkPipeline pipeline);
        ~VulkanComputePipeline() override;

        VkPipeline pipeline() const;

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkPipeline m_pipeline = VK_NULL_HANDLE;
    };

} // namespace hyper_engine