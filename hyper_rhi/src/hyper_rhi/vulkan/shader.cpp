/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/shader.hpp"

#include "hyper_rhi/vulkan/graphics_device.hpp"
#include "hyper_rhi/vulkan/utils.hpp"

namespace he {

VulkanShader::VulkanShader(VulkanGraphicsDevice &graphics_device, const ShaderDescriptor &desc)
    : m_graphics_device(graphics_device)
{
    const auto shader_module_create_info = VkShaderModuleCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = desc.byte_code.size(),
        .pCode = reinterpret_cast<const uint32_t *>(desc.byte_code.data()),
    };

    HE_VK_CHECK(
        vkCreateShaderModule(m_graphics_device.device(), &shader_module_create_info, nullptr, &m_raw),
        "Failed to create vulkan shader module");
}

VulkanShader::~VulkanShader() { vkDestroyShaderModule(m_graphics_device.device(), m_raw, nullptr); }

} // namespace he
