/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

// clang-format off
#include <volk.h>
#include <vk_mem_alloc.h>
// clang-format on

#include "hyper_rhi/texture.hpp"

namespace he {

class VulkanGraphicsDevice;

class VulkanTexture {
public:
    VulkanTexture(VulkanGraphicsDevice &, const TextureDescriptor &, VkImage image);
    ~VulkanTexture();

    VkImage raw() const { return m_raw; }

private:
    VulkanGraphicsDevice &m_graphics_device;

    VkImage m_raw { VK_NULL_HANDLE };
    VmaAllocation m_allocation { VK_NULL_HANDLE };
};

} // namespace he
