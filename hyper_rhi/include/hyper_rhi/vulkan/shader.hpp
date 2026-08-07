/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "hyper_rhi/shader.hpp"

namespace he {

class VulkanGraphicsDevice;

class VulkanShader {
public:
    VulkanShader(VulkanGraphicsDevice &, const ShaderDescriptor &);
    ~VulkanShader();

    VkShaderModule raw() const { return m_raw; }

private:
    VulkanGraphicsDevice &m_graphics_device;

    VkShaderModule m_raw = VK_NULL_HANDLE;
};

} // namespace he
