/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "core/memory.hpp"
#include "core/prerequisites.hpp"
#include "rhi/compute_pipeline.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanComputePipeline final : public ComputePipeline
    {
    public:
        static RefPtr<VulkanComputePipeline> create(VulkanGraphicsDevice &, const ComputePipelineDescriptor &);
        ~VulkanComputePipeline() override;

        HE_ALWAYS_INLINE VkPipeline raw() const { return m_raw; }

    private:
        explicit VulkanComputePipeline(VulkanGraphicsDevice &graphics_device, const ComputePipelineDescriptor &desc)
            : ComputePipeline(desc)
            , m_graphics_device(graphics_device)
        {
        }

        bool initialize(const ComputePipelineDescriptor &);

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkPipeline m_raw = VK_NULL_HANDLE;
    };
} // namespace he
