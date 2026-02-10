/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <hyper_core/prerequisites.hpp>

#include "hyper_rhi/pipeline_layout.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanPipelineLayout
    {
    public:
        VulkanPipelineLayout(VulkanGraphicsDevice &, const PipelineLayoutDescriptor &);
        ~VulkanPipelineLayout();

        HE_ALWAYS_INLINE VkPipelineLayout raw() const { return m_raw; }

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkPipelineLayout m_raw = VK_NULL_HANDLE;
    };
} // namespace he
