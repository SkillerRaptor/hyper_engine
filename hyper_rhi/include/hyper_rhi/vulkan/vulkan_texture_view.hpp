/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "hyper_rhi/texture_view.hpp"

namespace he {

class VulkanTextureView : public TextureView {
public:
    VulkanTextureView(const TextureViewDescriptor &, VkDevice);
    ~VulkanTextureView() override;

    VkImageView raw() const { return m_raw; }

private:
    VkDevice m_device = VK_NULL_HANDLE;

    VkImageView m_raw = VK_NULL_HANDLE;
};

} // namespace he
