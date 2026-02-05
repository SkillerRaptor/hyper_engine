/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/sampler.hpp"

#include "hyper_rhi/validation.hpp"

namespace he
{
    VulkanSampler::VulkanSampler(VulkanGraphicsDevice &graphics_device, const SamplerDescriptor &desc)
        : Sampler(desc)
        , m_graphics_device(graphics_device)
    {
        validate_sampler_descriptor(desc);
    }

    VulkanSampler::~VulkanSampler() { }
} // namespace he
