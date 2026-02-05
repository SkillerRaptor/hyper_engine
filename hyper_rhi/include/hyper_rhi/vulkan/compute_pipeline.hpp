/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <hyper_core/prerequisites.hpp>

#include "hyper_rhi/compute_pipeline.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanComputePipeline final : public ComputePipeline
    {
    public:
        VulkanComputePipeline(VulkanGraphicsDevice &, const ComputePipelineDescriptor &);
        ~VulkanComputePipeline() override;

        HE_ALWAYS_INLINE VkPipeline raw() const { return m_raw; }

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkPipeline m_raw = VK_NULL_HANDLE;
    };
} // namespace he
