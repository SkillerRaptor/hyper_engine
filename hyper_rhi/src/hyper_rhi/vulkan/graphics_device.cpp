/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/graphics_device.hpp"

#include <array>
#include <vector>

#include <GLFW/glfw3.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

namespace hyper_rhi
{
    static const std::array<const char *, 1> g_validation_layers = {
        "VK_LAYER_KHRONOS_validation",
    };

    VulkanGraphicsDevice::VulkanGraphicsDevice(const GraphicsDeviceDescriptor &descriptor)
        : m_validation_layers_enabled(false)
        , m_instance(nullptr)
        , m_debug_messenger(nullptr)
    {
        volkInitialize();

        if (descriptor.debug_mode)
        {
            if (this->check_validation_layer_support())
            {
                m_validation_layers_enabled = true;
            }
            else
            {
                HE_WARN("Validation layers were requested, but not available!");
                m_validation_layers_enabled = false;
            }
        }

        this->create_instance();
        this->create_debug_messenger();
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        if (m_validation_layers_enabled)
        {
            vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        }

        vkDestroyInstance(m_instance, nullptr);
    }

    bool VulkanGraphicsDevice::check_validation_layer_support()
    {
        uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> layer_properties(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data());

        for (const char *layer_name : g_validation_layers)
        {
            bool layer_found = false;

            for (const VkLayerProperties &properties : layer_properties)
            {
                if (std::strcmp(layer_name, properties.layerName) == 0)
                {
                    layer_found = true;
                    break;
                }
            }

            if (!layer_found)
            {
                return false;
            }
        }

        return true;
    }

    void VulkanGraphicsDevice::create_instance()
    {
        const VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debug_callback,
            .pUserData = nullptr,
        };

        const void *next = m_validation_layers_enabled ? &debug_create_info : nullptr;

        const VkApplicationInfo application_info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "HyperEngine",
            .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .pEngineName = "HyperEngine",
            .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .apiVersion = VK_API_VERSION_1_3,
        };

        const uint32_t layer_count = m_validation_layers_enabled ? static_cast<uint32_t>(g_validation_layers.size()) : 0;
        const char *const *layers = m_validation_layers_enabled ? g_validation_layers.data() : nullptr;

        uint32_t required_extension_count = 0;
        const char *const *required_extensions = glfwGetRequiredInstanceExtensions(&required_extension_count);

        std::vector<const char *> extensions(required_extensions, required_extensions + required_extension_count);
        if (m_validation_layers_enabled)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        const VkInstanceCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = next,
            .flags = 0,
            .pApplicationInfo = &application_info,
            .enabledLayerCount = layer_count,
            .ppEnabledLayerNames = layers,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
        };

        HE_VK_CHECK(vkCreateInstance(&create_info, nullptr, &m_instance));
        HE_ASSERT(m_instance);

        volkLoadInstance(m_instance);
    }

    void VulkanGraphicsDevice::create_debug_messenger()
    {
        if (!m_validation_layers_enabled)
        {
            return;
        }

        const VkDebugUtilsMessengerCreateInfoEXT create_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debug_callback,
            .pUserData = nullptr,
        };

        HE_VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_instance, &create_info, nullptr, &m_debug_messenger));
        HE_ASSERT(m_debug_messenger);
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanGraphicsDevice::debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void *)
    {
        switch (message_severity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            HE_TRACE("Validation Layer: {}", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            HE_INFO("Validation Layer: {}", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            HE_WARN("Validation Layer: {}", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            HE_ERROR("Validation Layer: {}", callback_data->pMessage);
            break;
        default:
            break;
        }

        return VK_FALSE;
    }
} // namespace hyper_rhi
