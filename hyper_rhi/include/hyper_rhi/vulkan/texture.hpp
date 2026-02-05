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

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanTexture final : public Texture
    {
    public:
        VulkanTexture(VulkanGraphicsDevice &, const TextureDescriptor &);
        ~VulkanTexture() override;

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkImage m_raw = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
        VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageView m_main_view = VK_NULL_HANDLE;
    };
} // namespace he
