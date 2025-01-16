/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/vulkan/vulkan_common.hpp"

namespace hyper_engine
{
    class VulkanGraphicsDevice;

    class VulkanPipelineLayout final : public PipelineLayout
    {
    public:
        VulkanPipelineLayout(
            const PipelineLayoutDescriptor &descriptor,
            VulkanGraphicsDevice &graphics_device,
            VkPipelineLayout pipeline_layout);
        ~VulkanPipelineLayout() override;

        VkPipelineLayout pipeline_layout() const;

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
    };
} // namespace hyper_engine