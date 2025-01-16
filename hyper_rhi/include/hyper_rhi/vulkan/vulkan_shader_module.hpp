/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_rhi/shader_module.hpp"
#include "hyper_rhi/vulkan/vulkan_common.hpp"

namespace hyper_engine
{
    class VulkanGraphicsDevice;

    class VulkanShaderModule final : public ShaderModule
    {
    public:
        VulkanShaderModule(const ShaderModuleDescriptor &descriptor, VulkanGraphicsDevice &graphics_device, VkShaderModule shader_module);
        ~VulkanShaderModule() override;

        VkShaderModule shader_module() const;

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkShaderModule m_shader_module = VK_NULL_HANDLE;
    };
} // namespace hyper_engine