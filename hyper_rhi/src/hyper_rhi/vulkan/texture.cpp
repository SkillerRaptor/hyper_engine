/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/texture.hpp"

#include "hyper_rhi/validation.hpp"

namespace he
{
    VulkanTexture::VulkanTexture(VulkanGraphicsDevice &graphics_device, const TextureDescriptor &desc)
        : Texture(desc)
        , m_graphics_device(graphics_device)
    {
        validate_texture_descriptor(desc);
    }

    VulkanTexture::~VulkanTexture() { }
} // namespace he
