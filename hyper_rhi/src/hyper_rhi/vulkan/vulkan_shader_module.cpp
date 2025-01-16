/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_shader_module.hpp"

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"

namespace hyper_engine
{
    RefPtr<ShaderModule> VulkanGraphicsDevice::create_shader_module_platform(const ShaderModuleDescriptor &descriptor)
    {
        const VkShaderModuleCreateInfo shader_module_create_info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = descriptor.bytes.size(),
            .pCode = reinterpret_cast<const uint32_t *>(descriptor.bytes.data()),
        };

        VkShaderModule shader_module = VK_NULL_HANDLE;
        HE_VK_CHECK(vkCreateShaderModule(m_device, &shader_module_create_info, nullptr, &shader_module));

        HE_ASSERT(shader_module != VK_NULL_HANDLE);

        set_object_name(shader_module, ObjectType::ShaderModule, descriptor.label);

        return make_ref<VulkanShaderModule>(descriptor, *this, shader_module);
    }

    VulkanShaderModule::VulkanShaderModule(
        const ShaderModuleDescriptor &descriptor,
        VulkanGraphicsDevice &graphics_device,
        const VkShaderModule shader_module)
        : ShaderModule(descriptor)
        , m_graphics_device(graphics_device)
        , m_shader_module(shader_module)
    {
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        m_graphics_device.resource_queue().shader_modules.emplace_back(m_shader_module);
    }

    VkShaderModule VulkanShaderModule::shader_module() const
    {
        return m_shader_module;
    }
} // namespace hyper_engine