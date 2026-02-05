/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <hyper_core/prerequisites.hpp>

#include "hyper_rhi/sampler.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanSampler final : public Sampler
    {
    public:
        VulkanSampler(VulkanGraphicsDevice &, const SamplerDescriptor &);
        ~VulkanSampler() override;

        HE_ALWAYS_INLINE VkSampler raw() const { return m_raw; }

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkSampler m_raw = VK_NULL_HANDLE;
    };
} // namespace he
