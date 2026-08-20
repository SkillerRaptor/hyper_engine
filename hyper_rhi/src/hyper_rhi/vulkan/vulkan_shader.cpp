/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_shader.hpp"

#include "hyper_rhi/vulkan/vulkan_macros.hpp"

namespace he {

VulkanShader::VulkanShader(const ShaderDescriptor &desc, const VkDevice device)
    : Shader(desc)
    , m_device(device)
{
    const VkShaderModuleCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = desc.byte_code.size(),
        .pCode = reinterpret_cast<const uint32_t *>(desc.byte_code.data()),
    };

    HE_VK_CHECK(vkCreateShaderModule(m_device, &create_info, nullptr, &m_raw));
}

VulkanShader::~VulkanShader() { vkDestroyShaderModule(m_device, m_raw, nullptr); }

} // namespace he
