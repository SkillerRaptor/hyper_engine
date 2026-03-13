/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "hyper_rhi/texture_view.hpp"

namespace he {

class VulkanGraphicsDevice;

class VulkanTextureView {
public:
    VulkanTextureView(VulkanGraphicsDevice &, const TextureViewDescriptor &);
    ~VulkanTextureView();

    VkImageView raw() const { return m_raw; }

private:
    VulkanGraphicsDevice &m_graphics_device;

    VkImageView m_raw { VK_NULL_HANDLE };
};

} // namespace he
