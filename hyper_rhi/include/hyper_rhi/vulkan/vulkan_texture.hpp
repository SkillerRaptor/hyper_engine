/*
 * Copyright (c) 2026-present, SkillerRaptor
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

class VulkanTexture : public Texture {
public:
    VulkanTexture(const TextureDescriptor &, VmaAllocator);
    ~VulkanTexture() override;

    VkImage raw() const { return m_raw; }
    VmaAllocation allocation() const { return m_allocation; }

private:
    VmaAllocator m_allocator = VK_NULL_HANDLE;

    VkImage m_raw = VK_NULL_HANDLE;
    VmaAllocation m_allocation = VK_NULL_HANDLE;
};

} // namespace he
