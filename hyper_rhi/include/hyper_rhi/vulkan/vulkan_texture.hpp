/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_rhi/texture.hpp"
#include "hyper_rhi/vulkan/vulkan_common.hpp"

#include <vk_mem_alloc.h>

namespace hyper_engine
{
    class VulkanGraphicsDevice;

    class VulkanTexture final : public Texture
    {
    public:
        VulkanTexture(const TextureDescriptor &descriptor,
            VulkanGraphicsDevice &graphics_device, VkImage image, VmaAllocation allocation);
        ~VulkanTexture() override;

        VkImage image() const;
        VmaAllocation allocation() const;

        static Format format_to_texture_format(VkFormat format);
        static VkFormat get_format(Format format);
        static VkImageType get_image_type(Dimension dimension);
        static VkImageUsageFlags get_image_usage_flags(BitFlags<TextureUsage> texture_usage_flags, Format format);

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkImage m_image = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
    };
} // namespace hyper_engine