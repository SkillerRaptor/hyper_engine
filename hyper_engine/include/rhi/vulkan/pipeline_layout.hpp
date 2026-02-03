/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <hyper_core/memory.hpp>
#include <hyper_core/prerequisites.hpp>

#include "rhi/pipeline_layout.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanPipelineLayout final : public PipelineLayout
    {
    public:
        static RefPtr<VulkanPipelineLayout> create(VulkanGraphicsDevice &, const PipelineLayoutDescriptor &);
        ~VulkanPipelineLayout() override;

        HE_ALWAYS_INLINE VkPipelineLayout raw() const { return m_raw; }

    private:
        explicit VulkanPipelineLayout(VulkanGraphicsDevice &graphics_device, const PipelineLayoutDescriptor &desc)
            : PipelineLayout(desc)
            , m_graphics_device(graphics_device)
        {
        }

        bool initialize(const PipelineLayoutDescriptor &);

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkPipelineLayout m_raw = VK_NULL_HANDLE;
    };
} // namespace he
