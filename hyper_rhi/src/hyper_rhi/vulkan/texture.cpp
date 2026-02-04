/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/texture.hpp"

#include <hyper_core/logger.hpp>

#include "hyper_rhi/validation.hpp"

namespace he
{
    RefPtr<VulkanTexture> VulkanTexture::create(VulkanGraphicsDevice &graphics_device, const TextureDescriptor &desc)
    {
        RefPtr<VulkanTexture> texture = wrap_ref<VulkanTexture>(new VulkanTexture(graphics_device, desc));
        if (!texture->initialize(desc))
        {
            HE_ERROR("Failed to initialize texture");
            return nullptr;
        }

        return texture;
    }

    VulkanTexture::~VulkanTexture() { }

    bool VulkanTexture::initialize(const TextureDescriptor &desc)
    {
        if (!validate_texture_descriptor(desc))
        {
            return false;
        }

        return true;
    }
} // namespace he
