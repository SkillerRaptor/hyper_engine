/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <hyper_core/memory.hpp>
#include <hyper_core/prerequisites.hpp>

#include "hyper_rhi/sampler.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanSampler final : public Sampler
    {
    public:
        static RefPtr<VulkanSampler> create(VulkanGraphicsDevice &, const SamplerDescriptor &);
        ~VulkanSampler() override;

        HE_ALWAYS_INLINE VkSampler raw() const { return m_raw; }

    private:
        explicit VulkanSampler(VulkanGraphicsDevice &graphics_device, const SamplerDescriptor &desc)
            : Sampler(desc)
            , m_graphics_device { graphics_device }
        {
        }

        bool initialize(const SamplerDescriptor &);

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkSampler m_raw = VK_NULL_HANDLE;
    };
} // namespace he
