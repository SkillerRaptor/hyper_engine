/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "hyper_rhi/shader.hpp"

namespace he {

class VulkanShader : public Shader {
public:
    VulkanShader(const ShaderDescriptor &, VkDevice);
    ~VulkanShader() override;

    VkShaderModule raw() const { return m_raw; }

private:
    VkDevice m_device = VK_NULL_HANDLE;

    VkShaderModule m_raw = VK_NULL_HANDLE;
};

} // namespace he
