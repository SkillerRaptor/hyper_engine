/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "rhi/vulkan/sampler.hpp"

#include <hyper_core/logger.hpp>

#include "rhi/validation.hpp"

namespace he
{
    RefPtr<VulkanSampler> VulkanSampler::create(VulkanGraphicsDevice &graphics_device, const SamplerDescriptor &desc)
    {
        RefPtr<VulkanSampler> sampler = wrap_ref<VulkanSampler>(new VulkanSampler(graphics_device, desc));
        if (!sampler->initialize(desc))
        {
            HE_ERROR("Failed to initialize sampler");
            return nullptr;
        }

        return sampler;
    }

    VulkanSampler::~VulkanSampler() { }

    bool VulkanSampler::initialize(const SamplerDescriptor &desc)
    {
        if (!validate_sampler_descriptor(desc))
        {
            return false;
        }

        return true;
    }
} // namespace he
