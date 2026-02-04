/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <hyper_core/memory.hpp>
#include <hyper_core/prerequisites.hpp>

#include "hyper_rhi/render_pipeline.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanRenderPipeline final : public RenderPipeline
    {
    public:
        static RefPtr<VulkanRenderPipeline> create(VulkanGraphicsDevice &, const RenderPipelineDescriptor &);
        ~VulkanRenderPipeline() override;

        HE_ALWAYS_INLINE VkPipeline raw() const { return m_raw; }

    private:
        explicit VulkanRenderPipeline(VulkanGraphicsDevice &graphics_device, const RenderPipelineDescriptor &desc)
            : RenderPipeline(desc)
            , m_graphics_device(graphics_device)
        {
        }

        bool initialize(const RenderPipelineDescriptor &);

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkPipeline m_raw = VK_NULL_HANDLE;
    };
} // namespace he
