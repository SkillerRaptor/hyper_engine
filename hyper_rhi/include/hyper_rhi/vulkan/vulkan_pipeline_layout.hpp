/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <span>
#include <volk.h>

#include "hyper_rhi/pipeline_layout.hpp"

namespace he {

class VulkanPipelineLayout : public PipelineLayout {
public:
    VulkanPipelineLayout(const PipelineLayoutDescriptor &, VkDevice, std::span<const VkDescriptorSetLayout>);
    ~VulkanPipelineLayout() override;

    VkPipelineLayout raw() const { return m_raw; }

private:
    VkDevice m_device = VK_NULL_HANDLE;

    VkPipelineLayout m_raw = VK_NULL_HANDLE;
};

} // namespace he
