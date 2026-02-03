/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "rhi/vulkan/graphics_device.hpp"

#define VMA_IMPLEMENTATION
#include <array>

#include <SDL3/SDL_vulkan.h>
#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>

#include "core/assertion.hpp"
#include "core/logger.hpp"
#include "platform/window.hpp"
#include "rhi/vulkan/buffer.hpp"
#include "rhi/vulkan/compute_pipeline.hpp"
#include "rhi/vulkan/pipeline_layout.hpp"
#include "rhi/vulkan/render_pipeline.hpp"
#include "rhi/vulkan/sampler.hpp"
#include "rhi/vulkan/shader.hpp"
#include "rhi/vulkan/texture.hpp"

namespace he
{
    RefPtr<VulkanGraphicsDevice>
        VulkanGraphicsDevice::create(const Window &window, const Validation validation_requested)
    {
        const VkResult result = volkInitialize();
        if (result != VK_SUCCESS)
        {
            HE_ERROR("Failed to initialize volk ({})", string_VkResult(result));
            return nullptr;
        }

        RefPtr<VulkanGraphicsDevice> graphics_device = wrap_ref<VulkanGraphicsDevice>(new VulkanGraphicsDevice());
        if (!graphics_device->create_instance(validation_requested))
        {
            HE_ERROR("Failed to create instance");
            return nullptr;
        }

        if (!graphics_device->create_device())
        {
            HE_ERROR("Failed to create logical device");
            return nullptr;
        }

        if (!graphics_device->create_allocator())
        {
            HE_ERROR("Failed to create allocator");
            return nullptr;
        }

        if (!graphics_device->create_surface(window))
        {
            HE_ERROR("Failed to create surface");
            return nullptr;
        }

        if (!graphics_device->create_swapchain(window))
        {
            HE_ERROR("Failed to create swapchain");
            return nullptr;
        }

        if (!graphics_device->create_descriptors())
        {
            HE_ERROR("Failed to create descriptors");
            return nullptr;
        }

        HE_INFO("Created graphics device");

        return graphics_device;
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        vkDestroyDescriptorSetLayout(m_device, m_sampler_layout, nullptr);
        vkDestroyDescriptorSetLayout(m_device, m_storage_image_layout, nullptr);
        vkDestroyDescriptorSetLayout(m_device, m_sampled_image_layout, nullptr);
        vkDestroyDescriptorSetLayout(m_device, m_storage_buffer_layout, nullptr);
        vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);

        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

        vmaDestroyAllocator(m_allocator);
        vkDestroyDevice(m_device, nullptr);

        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        vkDestroyInstance(m_instance, nullptr);

        HE_INFO("Destroyed graphics device");
    }

    RefPtr<Buffer> VulkanGraphicsDevice::create_buffer(const BufferDescriptor &desc)
    {
        return VulkanBuffer::create(*this, desc);
    }

    RefPtr<Shader> VulkanGraphicsDevice::create_shader(const ShaderDescriptor &desc)
    {
        return VulkanShader::create(*this, desc);
    }

    RefPtr<Sampler> VulkanGraphicsDevice::create_sampler(const SamplerDescriptor &desc)
    {
        return VulkanSampler::create(*this, desc);
    }

    RefPtr<Texture> VulkanGraphicsDevice::create_texture(const TextureDescriptor &desc)
    {
        return VulkanTexture::create(*this, desc);
    }

    RefPtr<PipelineLayout> VulkanGraphicsDevice::create_pipeline_layout(const PipelineLayoutDescriptor &desc)
    {
        return VulkanPipelineLayout::create(*this, desc);
    }

    RefPtr<ComputePipeline> VulkanGraphicsDevice::create_compute_pipeline(const ComputePipelineDescriptor &desc)
    {
        return VulkanComputePipeline::create(*this, desc);
    }

    RefPtr<RenderPipeline> VulkanGraphicsDevice::create_render_pipeline(const RenderPipelineDescriptor &desc)
    {
        return VulkanRenderPipeline::create(*this, desc);
    }

    bool VulkanGraphicsDevice::create_instance(const Validation validation_requested)
    {
        static constexpr const char *s_validation_layer = "VK_LAYER_KHRONOS_validation";
        static constexpr const char *s_validation_extension = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

        VkResult result = VK_SUCCESS;

        const bool validation_enabled = [&]()
        {
            if (validation_requested == Validation::Disabled)
            {
                return false;
            }

            u32 layer_count = 0;
            result = vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to enumerate instance layer properties ({})", string_VkResult(result));
                return false;
            }

            std::vector<VkLayerProperties> layers(layer_count);
            result = vkEnumerateInstanceLayerProperties(&layer_count, layers.data());
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to enumerate instance layer properties ({})", string_VkResult(result));
                return false;
            }

            bool found_layer = false;
            for (const VkLayerProperties &properties : layers)
            {
                if (strcmp(properties.layerName, s_validation_layer) == 0)
                {
                    found_layer = true;
                    break;
                }
            }

            if (!found_layer)
            {
                HE_WARN("Failed to find requested validation layer");
                return false;
            }

            u32 extension_count = 0;
            result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to enumerate instance extension properties ({})", string_VkResult(result));
                return false;
            }

            std::vector<VkExtensionProperties> extensions(extension_count);
            result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to enumerate instance extension properties ({})", string_VkResult(result));
                return false;
            }

            bool found_extension = false;
            for (const VkExtensionProperties &properties : extensions)
            {
                if (strcmp(properties.extensionName, s_validation_extension) == 0)
                {
                    found_extension = true;
                    break;
                }
            }

            if (!found_extension)
            {
                HE_WARN("Failed to find requested validation layer extension");
                return false;
            }

            return true;
        }();

        constexpr VkApplicationInfo application_info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "HyperEngine",
            .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .pEngineName = "HyperEngine",
            .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .apiVersion = VK_API_VERSION_1_3,
        };

        constexpr VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType
            = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debug_callback,
            .pUserData = nullptr,
        };

        u32 required_extension_count = 0;
        const char *const *required_instance_extensions = SDL_Vulkan_GetInstanceExtensions(&required_extension_count);

        std::vector<const char *> extensions(
            required_instance_extensions, required_instance_extensions + required_extension_count);
        extensions.push_back(s_validation_extension);

        VkInstanceCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &application_info,
            .enabledLayerCount = 1,
            .ppEnabledLayerNames = &s_validation_layer,
            .enabledExtensionCount = static_cast<u32>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
        };

        if (validation_enabled)
        {
            create_info.pNext = &debug_create_info;
        }

        result = vkCreateInstance(&create_info, nullptr, &m_instance);
        if (result != VK_SUCCESS)
        {
            HE_ERROR("Failed to create vulkan instance ({})", string_VkResult(result));
            return false;
        }

        HE_ASSERT(m_instance != VK_NULL_HANDLE);

        volkLoadInstance(m_instance);

        if (validation_enabled)
        {
            result = vkCreateDebugUtilsMessengerEXT(m_instance, &debug_create_info, nullptr, &m_debug_messenger);
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to create vulkan debug messenger ({})", string_VkResult(result));
                return false;
            }

            HE_ASSERT(m_debug_messenger != VK_NULL_HANDLE);
        }

        return true;
    }

    bool VulkanGraphicsDevice::create_device()
    {
        static constexpr const char *s_swapchain_extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

        VkResult result = VK_SUCCESS;

        u32 physical_device_count = 0;
        result = vkEnumeratePhysicalDevices(m_instance, &physical_device_count, nullptr);
        if (result != VK_SUCCESS)
        {
            HE_ERROR("Failed to enumerate physical devices ({})", string_VkResult(result));
            return false;
        }

        if (physical_device_count == 0)
        {
            HE_ERROR("Failed to find physical devices with vulkan support");
            return false;
        }

        std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
        result = vkEnumeratePhysicalDevices(m_instance, &physical_device_count, physical_devices.data());
        if (result != VK_SUCCESS)
        {
            HE_ERROR("Failed to enumerate physical devices ({})", string_VkResult(result));
            return false;
        }

        VkPhysicalDeviceVulkan13Features features_13 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .pNext = nullptr,
        };

        VkPhysicalDeviceVulkan12Features features_12 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
            .pNext = &features_13,
        };

        VkPhysicalDeviceFeatures2 features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &features_12,
        };

        u32 highest_score = 0;
        for (u32 i = 0; i < physical_device_count; ++i)
        {
            const VkPhysicalDevice physical_device = physical_devices[i];

            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physical_device, &properties);

            HE_INFO("Evaluating device (name='{}')", properties.deviceName);

            // Queues
            u32 queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

            std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

            const std::optional<u32> queue_family_index = [&]() -> std::optional<u32>
            {
                u32 queue_index = 0;
                for (const VkQueueFamilyProperties &queue_family : queue_families)
                {
                    const bool graphics_supported = queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
                    const bool present_supported
                        = SDL_Vulkan_GetPresentationSupport(m_instance, physical_device, queue_index);

                    if (graphics_supported && present_supported)
                    {
                        return queue_index;
                    }

                    ++queue_index;
                }

                return std::nullopt;
            }();

            if (!queue_family_index.has_value())
            {
                HE_INFO("Skipped device due to missing queue with graphics and presentation support");
                continue;
            }

            // Extensions
            u32 extension_count = 0;
            result = vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to enumerate device extension properties ({})", string_VkResult(result));
                return false;
            }

            std::vector<VkExtensionProperties> extensions(extension_count);
            result
                = vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extensions.data());
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to enumerate device extension properties ({})", string_VkResult(result));
                return false;
            }

            const bool extension_found = [&]()
            {
                for (const VkExtensionProperties &extension_properties : extensions)
                {
                    if (strcmp(extension_properties.extensionName, s_swapchain_extension) == 0)
                    {
                        return true;
                    }
                }

                return false;
            }();

            if (!extension_found)
            {
                HE_INFO("Skipped device due to missing required extension (extension='{}')", s_swapchain_extension);
                continue;
            }

            vkGetPhysicalDeviceFeatures2(physical_device, &features);

#define REQUIRE_FEATURE(s, feature)                                                                      \
    if (s.feature == VK_FALSE)                                                                           \
    {                                                                                                    \
        HE_INFO("Skipped device due to missing required feature (feature='{}')", HE_STRINGIFY(feature)); \
        return false;                                                                                    \
    }

            REQUIRE_FEATURE(features.features, sampleRateShading);
            REQUIRE_FEATURE(features_12, shaderUniformBufferArrayNonUniformIndexing);
            REQUIRE_FEATURE(features_12, shaderSampledImageArrayNonUniformIndexing);
            REQUIRE_FEATURE(features_12, shaderStorageBufferArrayNonUniformIndexing);
            REQUIRE_FEATURE(features_12, shaderStorageImageArrayNonUniformIndexing);
            REQUIRE_FEATURE(features_12, descriptorBindingUniformBufferUpdateAfterBind);
            REQUIRE_FEATURE(features_12, descriptorBindingSampledImageUpdateAfterBind);
            REQUIRE_FEATURE(features_12, descriptorBindingStorageImageUpdateAfterBind);
            REQUIRE_FEATURE(features_12, descriptorBindingStorageBufferUpdateAfterBind);
            REQUIRE_FEATURE(features_12, descriptorBindingUpdateUnusedWhilePending);
            REQUIRE_FEATURE(features_12, descriptorBindingPartiallyBound);
            REQUIRE_FEATURE(features_12, descriptorBindingVariableDescriptorCount);
            REQUIRE_FEATURE(features_12, runtimeDescriptorArray);
            REQUIRE_FEATURE(features_12, timelineSemaphore);
            REQUIRE_FEATURE(features_13, synchronization2);
            REQUIRE_FEATURE(features_13, dynamicRendering);

#undef REQUIRE_FEATURE

            const VkPhysicalDeviceLimits limits = properties.limits;
            if (limits.timestampPeriod == 0.0f)
            {
                HE_INFO("Skipped device due to missing timestamp query support");
                return false;
            }

            if (limits.timestampComputeAndGraphics == VK_FALSE)
            {
                HE_INFO("Skipped device due to missing timestamp query support");
                return false;
            }

            const u32 device_type_score = [properties]()
            {
                switch (properties.deviceType)
                {
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    return 4;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    return 5;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    return 3;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    return 2;
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:;
                default:
                    return 1;
                }
            }();

            if (device_type_score > highest_score)
            {
                m_physical_device = physical_device;
                m_queue_family = queue_family_index.value();

                const VkSampleCountFlags counts
                    = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;
                if (counts & VK_SAMPLE_COUNT_64_BIT)
                {
                    m_sample_count = 64;
                }
                else if (counts & VK_SAMPLE_COUNT_32_BIT)
                {
                    m_sample_count = 32;
                }
                else if (counts & VK_SAMPLE_COUNT_16_BIT)
                {
                    m_sample_count = 16;
                }
                else if (counts & VK_SAMPLE_COUNT_8_BIT)
                {
                    m_sample_count = 8;
                }
                else if (counts & VK_SAMPLE_COUNT_4_BIT)
                {
                    m_sample_count = 4;
                }
                else if (counts & VK_SAMPLE_COUNT_2_BIT)
                {
                    m_sample_count = 2;
                }
                else
                {
                    m_sample_count = 1;
                }

                highest_score = device_type_score;
            }
        }

        if (m_physical_device == VK_NULL_HANDLE)
        {
            HE_ERROR("Failed to find a suitable physical device");
            return false;
        }

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(m_physical_device, &properties);

        const std::string_view device_type = [properties]()
        {
            switch (properties.deviceType)
            {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                return "Other";
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                return "Integrated GPU";
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                return "Discrete GPU";
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                return "Virtual GPU";
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                return "CPU";
            default:
                return "Unknown";
            }
        }();

        HE_INFO("Selected physical device (name='{}', type='{}')", properties.deviceName, device_type);

        constexpr f32 queue_priority = 1.0f;
        const VkDeviceQueueCreateInfo queue_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = m_queue_family,
            .queueCount = 1,
            .pQueuePriorities = &queue_priority,
        };

        VkDeviceCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &features,
            .flags = 0,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queue_create_info,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = 1,
            .ppEnabledExtensionNames = &s_swapchain_extension,
            .pEnabledFeatures = nullptr,
        };

        result = vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device);
        if (result != VK_SUCCESS)
        {
            HE_ERROR("Failed to create vulkan logical device ({})", string_VkResult(result));
            return false;
        }

        HE_ASSERT(m_device != VK_NULL_HANDLE);

        volkLoadDevice(m_device);

        vkGetDeviceQueue(m_device, m_queue_family, 0, &m_queue);
        HE_ASSERT(m_queue != VK_NULL_HANDLE);

        return true;
    }

    bool VulkanGraphicsDevice::create_allocator()
    {
        const VmaVulkanFunctions functions = {
            .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
            .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
            .vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
            .vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
            .vkAllocateMemory = vkAllocateMemory,
            .vkFreeMemory = vkFreeMemory,
            .vkMapMemory = vkMapMemory,
            .vkUnmapMemory = vkUnmapMemory,
            .vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges,
            .vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges,
            .vkBindBufferMemory = vkBindBufferMemory,
            .vkBindImageMemory = vkBindImageMemory,
            .vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements,
            .vkGetImageMemoryRequirements = vkGetImageMemoryRequirements,
            .vkCreateBuffer = vkCreateBuffer,
            .vkDestroyBuffer = vkDestroyBuffer,
            .vkCreateImage = vkCreateImage,
            .vkDestroyImage = vkDestroyImage,
            .vkCmdCopyBuffer = vkCmdCopyBuffer,
            .vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR,
            .vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR,
            .vkBindBufferMemory2KHR = vkBindBufferMemory2KHR,
            .vkBindImageMemory2KHR = vkBindImageMemory2KHR,
            .vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR,
            .vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements,
            .vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements,
        };

        const VmaAllocatorCreateInfo allocator_create_info = {
            .flags = 0,
            .physicalDevice = m_physical_device,
            .device = m_device,
            .preferredLargeHeapBlockSize = 0,
            .pAllocationCallbacks = nullptr,
            .pDeviceMemoryCallbacks = nullptr,
            .pHeapSizeLimit = nullptr,
            .pVulkanFunctions = &functions,
            .instance = m_instance,
            .vulkanApiVersion = VK_API_VERSION_1_3,
            .pTypeExternalMemoryHandleTypes = nullptr,
        };

        const VkResult result = vmaCreateAllocator(&allocator_create_info, &m_allocator);
        if (result != VK_SUCCESS)
        {
            HE_ERROR("Failed to create vulkan allocator ({})", string_VkResult(result));
            return false;
        }

        HE_ASSERT(m_allocator != VK_NULL_HANDLE);

        return true;
    }

    bool VulkanGraphicsDevice::create_surface(const Window &window)
    {
        if (!SDL_Vulkan_CreateSurface(window.native_handle(), m_instance, nullptr, &m_surface))
        {
            HE_ERROR("Failed to create vulkan surface ({})", SDL_GetError());
            return false;
        }

        HE_ASSERT(m_surface != VK_NULL_HANDLE);

        return true;
    }

    bool VulkanGraphicsDevice::create_swapchain(const Window &window)
    {
        VkResult result = VK_SUCCESS;

        const std::optional<VkExtent2D> extent = [&]() -> std::optional<VkExtent2D>
        {
            VkSurfaceCapabilitiesKHR capabilities;
            result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface, &capabilities);
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to get the physical device surface capabilities ({})", string_VkResult(result));
                return std::nullopt;
            }

            VkExtent2D current_extent = {
                .width = capabilities.currentExtent.width,
                .height = capabilities.currentExtent.height,
            };

            const auto [width, height] = window.size();
            if (current_extent.width == std::numeric_limits<u32>::max())
            {
                current_extent.width
                    = std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            }

            if (current_extent.height == std::numeric_limits<u32>::max())
            {
                current_extent.height
                    = std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
            }

            return current_extent;
        }();

        if (!extent.has_value())
        {
            HE_ERROR("Failed to choose swapchain extent");
            return false;
        }

        const std::optional<VkSurfaceFormatKHR> surface_format = [&]() -> std::optional<VkSurfaceFormatKHR>
        {
            u32 format_count = 0;
            result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &format_count, nullptr);
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to get the physical device surface formats ({})", string_VkResult(result));
                return std::nullopt;
            }

            std::vector<VkSurfaceFormatKHR> formats(format_count);
            result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &format_count, formats.data());
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to get the physical device surface formats ({})", string_VkResult(result));
                return std::nullopt;
            }

            for (const VkSurfaceFormatKHR format : formats)
            {
                if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    return format;
                }
            }

            HE_ERROR(
                "Failed to find a suitable surface format (format={}, color_space={})",
                string_VkFormat(VK_FORMAT_B8G8R8A8_UNORM),
                string_VkColorSpaceKHR(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR));

            return std::nullopt;
        }();

        if (!surface_format.has_value())
        {
            HE_ERROR("Failed to choose swapchain format");
            return false;
        }

        const std::optional<VkPresentModeKHR> present_mode = [&]() -> std::optional<VkPresentModeKHR>
        {
            u32 present_mode_count = 0;
            result
                = vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &present_mode_count, nullptr);
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to get the physical device surface present modes ({})", string_VkResult(result));
                return std::nullopt;
            }

            std::vector<VkPresentModeKHR> present_modes(present_mode_count);
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(
                m_physical_device, m_surface, &present_mode_count, present_modes.data());
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to get the physical device surface present modes ({})", string_VkResult(result));
                return std::nullopt;
            }

            for (const VkPresentModeKHR mode : present_modes)
            {
                if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    return mode;
                }
            }

            return present_modes[0];
        }();

        if (!present_mode.has_value())
        {
            HE_ERROR("Failed to choose swapchain present mode");
            return false;
        }

        VkSurfaceCapabilitiesKHR capabilities;
        result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface, &capabilities);
        if (result != VK_SUCCESS)
        {
            HE_ERROR("Failed to get the physical device surface capabilities ({})", string_VkResult(result));
            return false;
        }

        u32 min_image_count = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && min_image_count > capabilities.maxImageCount)
        {
            min_image_count = capabilities.maxImageCount;
        }

        const VkSwapchainCreateInfoKHR swapchain_create_info = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = m_surface,
            .minImageCount = min_image_count,
            .imageFormat = surface_format->format,
            .imageColorSpace = surface_format->colorSpace,
            .imageExtent = extent.value(),
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = present_mode.value(),
            .clipped = true,
            .oldSwapchain = m_swapchain,
        };

        result = vkCreateSwapchainKHR(m_device, &swapchain_create_info, nullptr, &m_swapchain);
        if (result != VK_SUCCESS)
        {
            HE_ERROR("Failed to create vulkan swapchain ({})", string_VkResult(result));
            return false;
        }

        HE_ASSERT(m_swapchain != VK_NULL_HANDLE);

        /*
           std::vector<VkImage> images(image_count);
           HE_VK_CHECK(vkGetSwapchainImagesKHR(device.raw, raw, &image_count, images.data()), vkGetSwapchainImagesKHR);

           const std::vector<TextureId> textures = images
               | std::views::transform(
                   [&](const VkImage image)
                   {
                       VulkanTexture *texture = new VulkanTexture {
                           .raw = image,
                           .allocation = VK_NULL_HANDLE,
                           .layout = VK_IMAGE_LAYOUT_UNDEFINED,
                       };
                       texture->views = {};

                       const TextureId texture_id(texture);

                       const Extent3d texture_extent {
                           .width = width,
                           .height = height,
                           .depth = 1,
                       };

                       const TextureDescriptor texture_desc {
                           .label = std::nullopt,
                           .extent = texture_extent,
                           .mip_levels = 1,
                           .format = map_vk_format(format.format),
                           .dimension = Dimension::D2,
                           .usage = TextureUsage::RenderAttachment,
                       };
                       texture->desc = texture_desc;

                       constexpr VkComponentMapping component_mapping {
                           .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                           .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                       };

                       const VkImageSubresourceRange subresource_range {
                           .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                           .baseMipLevel = 0,
                           .levelCount = 1,
                           .baseArrayLayer = 0,
                           .layerCount = 1,
                       };

                       const VkImageViewCreateInfo image_view_create_info {
                           .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                           .pNext = nullptr,
                           .flags = 0,
                           .image = texture->raw,
                           .viewType = VK_IMAGE_VIEW_TYPE_2D,
                           .format = format.format,
                           .components = component_mapping,
                           .subresourceRange = subresource_range,
                       };

                       VkImageView image_view { VK_NULL_HANDLE };
                       HE_VK_CHECK(vkCreateImageView(device.raw, &image_view_create_info, nullptr, &image_view),
        vkCreateImageView); HE_ASSERT(image_view != VK_NULL_HANDLE);

                       VulkanTextureView *texture_view = new VulkanTextureView {
                           .raw = image_view,
                       };
                       const TextureViewId texture_view_id(texture_view);

                       const TextureViewDescriptor texture_view_desc {
                           .label = std::nullopt,
                           .texture = texture_id,
                           .dimension = ViewDimension::D2,
                           .base_mip_level = 0,
                           .mip_levels = 1,
                           .base_array_layer = 0,
                           .array_layers = 1,
                       };
                       texture_view->desc = texture_view_desc;

                       texture->views.push_back(texture_view_id);

                       return texture_id;
                   })
               | std::ranges::to<std::vector<TextureId>>();
               */

        return true;
    }

    bool VulkanGraphicsDevice::create_descriptors()
    {
        VkResult result = VK_SUCCESS;

        std::array<VkDescriptorPoolSize, 4> pool_sizes = {
            VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 1000 },
            VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  .descriptorCount = 1000 },
            VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,  .descriptorCount = 1000 },
            VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_SAMPLER,        .descriptorCount = 1000 },
        };

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(m_physical_device, &properties);
        for (VkDescriptorPoolSize &descriptor_pool_size : pool_sizes)
        {
            const u32 limit = [&]()
            {
                switch (descriptor_pool_size.type)
                {
                case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                    return properties.limits.maxDescriptorSetStorageBuffers;
                case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                    return properties.limits.maxDescriptorSetSampledImages;
                case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                    return properties.limits.maxDescriptorSetStorageImages;
                case VK_DESCRIPTOR_TYPE_SAMPLER:
                    return properties.limits.maxDescriptorSetSamplers;
                default:
                    HE_UNREACHABLE();
                }
            }();

            HE_ASSERT(limit > 0);

            if (descriptor_pool_size.descriptorCount > limit)
            {
                descriptor_pool_size.descriptorCount = limit;
            }
        }

        const VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
            .maxSets = static_cast<u32>(pool_sizes.size()),
            .poolSizeCount = static_cast<u32>(pool_sizes.size()),
            .pPoolSizes = pool_sizes.data(),
        };

        result = vkCreateDescriptorPool(m_device, &descriptor_pool_create_info, nullptr, &m_descriptor_pool);
        if (result != VK_SUCCESS)
        {
            HE_ERROR("Failed to create vulkan descriptor pool ({})", string_VkResult(result));
            return false;
        }

        HE_ASSERT(m_descriptor_pool != VK_NULL_HANDLE);

        for (const VkDescriptorPoolSize descriptor_pool_size : pool_sizes)
        {
            const VkDescriptorSetLayoutBinding descriptor_set_layout_binding = {
                .binding = 0,
                .descriptorType = descriptor_pool_size.type,
                .descriptorCount = descriptor_pool_size.descriptorCount,
                .stageFlags = VK_SHADER_STAGE_ALL,
                .pImmutableSamplers = nullptr,
            };

            constexpr VkDescriptorBindingFlags descriptor_binding_flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
                | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

            VkDescriptorSetLayoutBindingFlagsCreateInfo descriptor_set_layout_binding_flags_info = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
                .pNext = nullptr,
                .bindingCount = 1,
                .pBindingFlags = &descriptor_binding_flags,
            };

            const VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .pNext = &descriptor_set_layout_binding_flags_info,
                .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
                .bindingCount = 1,
                .pBindings = &descriptor_set_layout_binding,
            };

            VkDescriptorSetLayout *descriptor_set_layout = [this, descriptor_pool_size]()
            {
                switch (descriptor_pool_size.type)
                {
                case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                    return &m_storage_buffer_layout;
                case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                    return &m_sampled_image_layout;
                case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                    return &m_storage_image_layout;
                case VK_DESCRIPTOR_TYPE_SAMPLER:
                    return &m_sampler_layout;
                default:
                    HE_UNREACHABLE();
                }
            }();

            result = vkCreateDescriptorSetLayout(
                m_device, &descriptor_set_layout_create_info, nullptr, descriptor_set_layout);
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to create vulkan descriptor set layout ({})", string_VkResult(result));
                return false;
            }

            HE_ASSERT(descriptor_set_layout != VK_NULL_HANDLE);

            VkDescriptorSetVariableDescriptorCountAllocateInfo descriptor_set_variable_descriptor_count_info = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
                .pNext = nullptr,
                .descriptorSetCount = 1,
                .pDescriptorCounts = &descriptor_pool_size.descriptorCount,
            };

            const VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .pNext = &descriptor_set_variable_descriptor_count_info,
                .descriptorPool = m_descriptor_pool,
                .descriptorSetCount = 1,
                .pSetLayouts = descriptor_set_layout,
            };

            VkDescriptorSet *descriptor_set = [this, descriptor_pool_size]()
            {
                switch (descriptor_pool_size.type)
                {
                case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                    return &m_storage_buffer_set;
                case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                    return &m_sampled_image_set;
                case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                    return &m_storage_image_set;
                case VK_DESCRIPTOR_TYPE_SAMPLER:
                    return &m_sampler_set;
                default:
                    HE_UNREACHABLE();
                }
            }();

            result = vkAllocateDescriptorSets(m_device, &descriptor_set_allocate_info, descriptor_set);
            if (result != VK_SUCCESS)
            {
                HE_ERROR("Failed to allocate vulkan descriptor set ({})", string_VkResult(result));
                return false;
            }

            HE_ASSERT(descriptor_set != VK_NULL_HANDLE);
        }

        return true;
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
            HE_TRACE("{}", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            HE_INFO("{}", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            HE_WARN("{}", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            HE_ERROR("{}", callback_data->pMessage);
            break;
        default:
            break;
        }

        return VK_FALSE;
    }
} // namespace he
