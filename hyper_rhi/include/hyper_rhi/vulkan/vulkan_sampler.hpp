/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "hyper_rhi/sampler.hpp"

namespace he {

class VulkanSampler : public Sampler {
public:
    VulkanSampler(const SamplerDescriptor &, VkDevice);
    ~VulkanSampler() override;

    VkSampler raw() const { return m_raw; }

private:
    VkDevice m_device = VK_NULL_HANDLE;

    VkSampler m_raw = VK_NULL_HANDLE;
};

} // namespace he
