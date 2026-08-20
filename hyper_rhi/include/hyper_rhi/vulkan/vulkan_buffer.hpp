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

#include "hyper_rhi/buffer.hpp"

namespace he {

class VulkanBuffer : public Buffer {
public:
    VulkanBuffer(const BufferDescriptor &, VmaAllocator);
    ~VulkanBuffer() override;

    VkBuffer raw() const { return m_raw; }
    VmaAllocation allocation() const { return m_allocation; }

private:
    VmaAllocator m_allocator = VK_NULL_HANDLE;

    VkBuffer m_raw = VK_NULL_HANDLE;
    VmaAllocation m_allocation = VK_NULL_HANDLE;
};

} // namespace he
