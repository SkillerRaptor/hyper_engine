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

#include "core/memory.hpp"
#include "rhi/texture.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanTexture final : public Texture
    {
    public:
        static RefPtr<VulkanTexture> create(VulkanGraphicsDevice &, const TextureDescriptor &);
        ~VulkanTexture() override;

    private:
        explicit VulkanTexture(VulkanGraphicsDevice &graphics_device, const TextureDescriptor &desc)
            : Texture(desc)
            , m_graphics_device(graphics_device)
        {
        }

        bool initialize(const TextureDescriptor &);

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkImage m_raw = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
        VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageView m_main_view = VK_NULL_HANDLE;
    };
} // namespace he
