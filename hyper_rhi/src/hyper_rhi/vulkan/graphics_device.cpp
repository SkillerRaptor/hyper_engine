/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/graphics_device.hpp"

#include <array>
#include <map>
#include <set>
#include <vector>

#include <GLFW/glfw3.h>

#include "hyper_rhi/vulkan/surface.hpp"

namespace hyper_rhi
{
    static constexpr std::array<const char *, 1> g_validation_layers = {
        "VK_LAYER_KHRONOS_validation",
    };

    static constexpr std::array<const char *, 1> g_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    VulkanGraphicsDevice::VulkanGraphicsDevice(const GraphicsDeviceDescriptor &descriptor)
        : m_validation_layers_enabled(false)
        , m_instance(VK_NULL_HANDLE)
        , m_debug_messenger(VK_NULL_HANDLE)
        , m_physical_device(VK_NULL_HANDLE)
        , m_device(VK_NULL_HANDLE)
        , m_queue(VK_NULL_HANDLE)
    {
        volkInitialize();

        if (descriptor.debug_mode)
        {
            if (VulkanGraphicsDevice::check_validation_layer_support())
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
        this->choose_physical_device();
        this->create_device();
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        vkDestroyDevice(m_device, nullptr);

        if (m_validation_layers_enabled)
        {
            vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        }

        vkDestroyInstance(m_instance, nullptr);
    }

    VkInstance VulkanGraphicsDevice::instance() const
    {
        return m_instance;
    }

    VkPhysicalDevice VulkanGraphicsDevice::physical_device() const
    {
        return m_physical_device;
    }

    VkDevice VulkanGraphicsDevice::device() const
    {
        return m_device;
    }

    SurfaceHandle VulkanGraphicsDevice::create_surface(const SurfaceDescriptor &descriptor)
    {
        return std::make_shared<VulkanSurface>(*this, descriptor);
    }

    BufferHandle VulkanGraphicsDevice::create_buffer(const BufferDescriptor &descriptor)
    {
        HE_UNREACHABLE();
    }

    CommandListHandle VulkanGraphicsDevice::create_command_list()
    {
        HE_UNREACHABLE();
    }

    ComputePipelineHandle VulkanGraphicsDevice::create_compute_pipeline(const ComputePipelineDescriptor &descriptor)
    {
        HE_UNREACHABLE();
    }

    GraphicsPipelineHandle VulkanGraphicsDevice::create_graphics_pipeline(const GraphicsPipelineDescriptor &descriptor)
    {
        HE_UNREACHABLE();
    }

    PipelineLayoutHandle VulkanGraphicsDevice::create_pipeline_layout(const PipelineLayoutDescriptor &descriptor)
    {
        HE_UNREACHABLE();
    }

    ShaderModuleHandle VulkanGraphicsDevice::create_shader_module(const ShaderModuleDescriptor &descriptor)
    {
        HE_UNREACHABLE();
    }

    TextureHandle VulkanGraphicsDevice::create_texture(const TextureDescriptor &descriptor)
    {
        HE_UNREACHABLE();
    }

    bool VulkanGraphicsDevice::check_validation_layer_support()
    {
        uint32_t layer_count = 0;
        HE_VK_CHECK(vkEnumerateInstanceLayerProperties(&layer_count, nullptr));

        std::vector<VkLayerProperties> layer_properties(layer_count);
        HE_VK_CHECK(vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data()));

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
        constexpr VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {
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

        constexpr VkApplicationInfo application_info = {
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
        HE_ASSERT(m_instance != VK_NULL_HANDLE);

        volkLoadInstance(m_instance);
    }

    void VulkanGraphicsDevice::create_debug_messenger()
    {
        if (!m_validation_layers_enabled)
        {
            return;
        }

        constexpr VkDebugUtilsMessengerCreateInfoEXT create_info = {
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
        HE_ASSERT(m_debug_messenger != VK_NULL_HANDLE);
    }

    void VulkanGraphicsDevice::choose_physical_device()
    {
        uint32_t device_count = 0;
        HE_VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr));

        std::vector<VkPhysicalDevice> physical_devices(device_count);
        HE_VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &device_count, physical_devices.data()));

        std::multimap<uint32_t, VkPhysicalDevice> possible_physical_devices = {};
        for (const VkPhysicalDevice &physical_device : physical_devices)
        {
            const uint32_t score = VulkanGraphicsDevice::rate_physical_device(physical_device);
            possible_physical_devices.insert({ score, physical_device });
        }

        HE_ASSERT(possible_physical_devices.rbegin()->first != 0);

        m_physical_device = possible_physical_devices.rbegin()->second;
    }

    uint32_t VulkanGraphicsDevice::rate_physical_device(const VkPhysicalDevice &physical_device) const
    {
        uint32_t score = 0;

        const std::optional<uint32_t> queue_family = this->find_queue_family(physical_device);
        if (!queue_family.has_value())
        {
            return 0;
        }

        const bool extensions_supported = VulkanGraphicsDevice::check_extension_support(physical_device);
        if (!extensions_supported)
        {
            return 0;
        }

        const bool features_supported = VulkanGraphicsDevice::check_feature_support(physical_device);
        if (!features_supported)
        {
            return 0;
        }

        VkPhysicalDeviceProperties device_properties = {};
        vkGetPhysicalDeviceProperties(physical_device, &device_properties);

        switch (device_properties.deviceType)
        {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            score += 0;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            score += 500;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            score += 1000;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            score += 250;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            score += 100;
            break;
        default:
            HE_UNREACHABLE();
        }

        return score;
    }

    std::optional<uint32_t> VulkanGraphicsDevice::find_queue_family(const VkPhysicalDevice &physical_device) const
    {
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

        uint32_t index = 0;
        for (const VkQueueFamilyProperties &queue_family : queue_families)
        {
            const bool graphics_supported = queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
            const bool present_supported = glfwGetPhysicalDevicePresentationSupport(m_instance, physical_device, index);

            if (graphics_supported && present_supported)
            {
                return index;
            }

            ++index;
        }

        return std::nullopt;
    }

    bool VulkanGraphicsDevice::check_extension_support(const VkPhysicalDevice &physical_device)
    {
        uint32_t extension_count = 0;
        HE_VK_CHECK(vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr));

        std::vector<VkExtensionProperties> extensions(extension_count);
        HE_VK_CHECK(vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extensions.data()));

        std::set<std::string> required_extensions(g_device_extensions.begin(), g_device_extensions.end());
        for (const VkExtensionProperties &extension : extensions)
        {
            required_extensions.erase(extension.extensionName);
        }

        return required_extensions.empty();
    }

    bool VulkanGraphicsDevice::check_feature_support(const VkPhysicalDevice &physical_device)
    {
        VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .pNext = nullptr,
            .dynamicRendering = VK_FALSE,
        };

        VkPhysicalDeviceTimelineSemaphoreFeatures timeline_semaphore = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
            .pNext = &dynamic_rendering,
            .timelineSemaphore = VK_FALSE,
        };

        VkPhysicalDeviceSynchronization2Features synchronization2 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
            .pNext = &timeline_semaphore,
            .synchronization2 = VK_FALSE,
        };

        VkPhysicalDeviceDescriptorIndexingFeatures descriptor_indexing = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
            .pNext = &synchronization2,
            .shaderInputAttachmentArrayDynamicIndexing = VK_FALSE,
            .shaderUniformTexelBufferArrayDynamicIndexing = VK_FALSE,
            .shaderStorageTexelBufferArrayDynamicIndexing = VK_FALSE,
            .shaderUniformBufferArrayNonUniformIndexing = VK_FALSE,
            .shaderSampledImageArrayNonUniformIndexing = VK_FALSE,
            .shaderStorageBufferArrayNonUniformIndexing = VK_FALSE,
            .shaderStorageImageArrayNonUniformIndexing = VK_FALSE,
            .shaderInputAttachmentArrayNonUniformIndexing = VK_FALSE,
            .shaderUniformTexelBufferArrayNonUniformIndexing = VK_FALSE,
            .shaderStorageTexelBufferArrayNonUniformIndexing = VK_FALSE,
            .descriptorBindingUniformBufferUpdateAfterBind = VK_FALSE,
            .descriptorBindingSampledImageUpdateAfterBind = VK_FALSE,
            .descriptorBindingStorageImageUpdateAfterBind = VK_FALSE,
            .descriptorBindingStorageBufferUpdateAfterBind = VK_FALSE,
            .descriptorBindingUniformTexelBufferUpdateAfterBind = VK_FALSE,
            .descriptorBindingStorageTexelBufferUpdateAfterBind = VK_FALSE,
            .descriptorBindingUpdateUnusedWhilePending = VK_FALSE,
            .descriptorBindingPartiallyBound = VK_FALSE,
            .descriptorBindingVariableDescriptorCount = VK_FALSE,
            .runtimeDescriptorArray = VK_FALSE,
        };

        VkPhysicalDeviceFeatures2 device_features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &descriptor_indexing,
            .features = {},
        };
        vkGetPhysicalDeviceFeatures2(physical_device, &device_features);

        const bool dynamic_rendering_supported = dynamic_rendering.dynamicRendering;
        const bool timeline_semaphore_supported = timeline_semaphore.timelineSemaphore;
        const bool synchronization2_supported = synchronization2.synchronization2;
        const bool descriptor_indexing_supported =
            descriptor_indexing.shaderUniformBufferArrayNonUniformIndexing & descriptor_indexing.shaderSampledImageArrayNonUniformIndexing &
            descriptor_indexing.shaderStorageBufferArrayNonUniformIndexing & descriptor_indexing.shaderStorageImageArrayNonUniformIndexing &
            descriptor_indexing.descriptorBindingUniformBufferUpdateAfterBind &
            descriptor_indexing.descriptorBindingSampledImageUpdateAfterBind & descriptor_indexing.descriptorBindingStorageImageUpdateAfterBind &
            descriptor_indexing.descriptorBindingStorageBufferUpdateAfterBind & descriptor_indexing.descriptorBindingUpdateUnusedWhilePending &
            descriptor_indexing.descriptorBindingPartiallyBound & descriptor_indexing.descriptorBindingVariableDescriptorCount &
            descriptor_indexing.runtimeDescriptorArray;

        const bool features_supported =
            dynamic_rendering_supported & timeline_semaphore_supported & synchronization2_supported & descriptor_indexing_supported;

        return features_supported;
    }

    void VulkanGraphicsDevice::create_device()
    {
        VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .pNext = nullptr,
            .dynamicRendering = VK_TRUE,
        };

        VkPhysicalDeviceTimelineSemaphoreFeatures timeline_semaphore = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
            .pNext = &dynamic_rendering,
            .timelineSemaphore = VK_TRUE,
        };

        VkPhysicalDeviceSynchronization2Features synchronization2 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
            .pNext = &timeline_semaphore,
            .synchronization2 = VK_TRUE,
        };

        VkPhysicalDeviceDescriptorIndexingFeatures descriptor_indexing = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
            .pNext = &synchronization2,
            .shaderInputAttachmentArrayDynamicIndexing = VK_TRUE,
            .shaderUniformTexelBufferArrayDynamicIndexing = VK_TRUE,
            .shaderStorageTexelBufferArrayDynamicIndexing = VK_TRUE,
            .shaderUniformBufferArrayNonUniformIndexing = VK_TRUE,
            .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
            .shaderStorageBufferArrayNonUniformIndexing = VK_TRUE,
            .shaderStorageImageArrayNonUniformIndexing = VK_TRUE,
            .shaderInputAttachmentArrayNonUniformIndexing = VK_TRUE,
            .shaderUniformTexelBufferArrayNonUniformIndexing = VK_TRUE,
            .shaderStorageTexelBufferArrayNonUniformIndexing = VK_TRUE,
            .descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE,
            .descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
            .descriptorBindingStorageImageUpdateAfterBind = VK_TRUE,
            .descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE,
            .descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE,
            .descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE,
            .descriptorBindingUpdateUnusedWhilePending = VK_TRUE,
            .descriptorBindingPartiallyBound = VK_TRUE,
            .descriptorBindingVariableDescriptorCount = VK_TRUE,
            .runtimeDescriptorArray = VK_TRUE,
        };

        VkPhysicalDeviceFeatures2 device_features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &descriptor_indexing,
            .features = {},
        };

        const std::optional<uint32_t> queue_family = VulkanGraphicsDevice::find_queue_family(m_physical_device);

        constexpr float queue_priority = 1.0f;
        const VkDeviceQueueCreateInfo queue_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queue_family.value(),
            .queueCount = 1,
            .pQueuePriorities = &queue_priority,
        };

        const uint32_t layer_count = m_validation_layers_enabled ? static_cast<uint32_t>(g_validation_layers.size()) : 0;
        const char *const *layers = m_validation_layers_enabled ? g_validation_layers.data() : nullptr;

        const VkDeviceCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &device_features,
            .flags = 0,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queue_create_info,
            .enabledLayerCount = layer_count,
            .ppEnabledLayerNames = layers,
            .enabledExtensionCount = static_cast<uint32_t>(g_device_extensions.size()),
            .ppEnabledExtensionNames = g_device_extensions.data(),
            .pEnabledFeatures = nullptr,
        };

        HE_VK_CHECK(vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device));
        HE_ASSERT(m_device != VK_NULL_HANDLE);

        vkGetDeviceQueue(m_device, queue_family.value(), 0, &m_queue);
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanGraphicsDevice::debug_callback(
        const VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        const VkDebugUtilsMessageTypeFlagsEXT,
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
