/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"

#define VMA_IMPLEMENTATION
#include <array>
#include <vk_mem_alloc.h>

#include <SDL3/SDL_vulkan.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>
#include <hyper_platform/window.hpp>

#include "hyper_rhi/vulkan/vulkan_buffer.hpp"
#include "hyper_rhi/vulkan/vulkan_compute_pipeline.hpp"
#include "hyper_rhi/vulkan/vulkan_macros.hpp"
#include "hyper_rhi/vulkan/vulkan_pipeline_layout.hpp"
#include "hyper_rhi/vulkan/vulkan_render_pipeline.hpp"
#include "hyper_rhi/vulkan/vulkan_sampler.hpp"
#include "hyper_rhi/vulkan/vulkan_shader.hpp"
#include "hyper_rhi/vulkan/vulkan_texture.hpp"
#include "hyper_rhi/vulkan/vulkan_texture_view.hpp"

namespace he {

VulkanGraphicsDevice::VulkanGraphicsDevice(const Window &window)
{
    HE_VK_CHECK(volkInitialize());

    create_instance();
    choose_physical_device();
    create_logical_device();
    create_allocator();
    create_surface(window);
    create_swapchain(window);
    create_descriptors();

    VkPhysicalDeviceProperties properties = { };
    vkGetPhysicalDeviceProperties(m_physical_device, &properties);

    const char *device_type = [properties]() {
        switch (properties.deviceType) {
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

    HE_INFO(
        "Created graphics device (gpu='{}', type='{}', api_version={}.{}.{})",
        properties.deviceName,
        device_type,
        VK_VERSION_MAJOR(properties.apiVersion),
        VK_VERSION_MINOR(properties.apiVersion),
        VK_VERSION_PATCH(properties.apiVersion));
}

VulkanGraphicsDevice::~VulkanGraphicsDevice()
{
    vkDestroyDescriptorSetLayout(m_device, m_sampler_layout, nullptr);
    vkDestroyDescriptorSetLayout(m_device, m_storage_image_layout, nullptr);
    vkDestroyDescriptorSetLayout(m_device, m_sampled_image_layout, nullptr);
    vkDestroyDescriptorSetLayout(m_device, m_storage_buffer_layout, nullptr);
    vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);

    for (const VkImageView image_view : m_swapchain_image_views) {
        vkDestroyImageView(m_device, image_view, nullptr);
    }

    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    vmaDestroyAllocator(m_allocator);
    vkDestroyDevice(m_device, nullptr);

    if (m_debug_messenger != VK_NULL_HANDLE) {
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
    }

    vkDestroyInstance(m_instance, nullptr);

    HE_INFO("Destroyed graphics device");
}

Buffer *VulkanGraphicsDevice::create_buffer_impl(const BufferDescriptor &desc)
{
    return new VulkanBuffer(desc, m_allocator);
}

void VulkanGraphicsDevice::destroy_buffer_impl(Buffer *buffer) { HE_PANIC(); }

void VulkanGraphicsDevice::bind_buffer_impl(const Buffer *buffer, const u32 slot) const
{
    const VulkanBuffer *vulkan_buffer = static_cast<const VulkanBuffer *>(buffer);

    const VkDescriptorBufferInfo descriptor_buffer_info = {
        .buffer = vulkan_buffer->raw(),
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };

    const VkWriteDescriptorSet write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_storage_buffer_set,
        .dstBinding = 0,
        .dstArrayElement = slot,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .pImageInfo = nullptr,
        .pBufferInfo = &descriptor_buffer_info,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);
}

Shader *VulkanGraphicsDevice::create_shader_impl(const ShaderDescriptor &desc)
{
    return new VulkanShader(desc, m_device);
}

void VulkanGraphicsDevice::destroy_shader_impl(Shader *shader) { HE_PANIC(); }

Sampler *VulkanGraphicsDevice::create_sampler_impl(const SamplerDescriptor &desc)
{
    return new VulkanSampler(desc, m_device);
}

void VulkanGraphicsDevice::destroy_sampler_impl(Sampler *sampler) { HE_PANIC(); }

void VulkanGraphicsDevice::bind_sampler_impl(const Sampler *sampler, const u32 slot) const
{
    const VulkanSampler *vulkan_sampler = static_cast<const VulkanSampler *>(sampler);

    const VkDescriptorImageInfo descriptor_image_info = {
        .sampler = vulkan_sampler->raw(),
        .imageView = VK_NULL_HANDLE,
        .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    const VkWriteDescriptorSet write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_sampler_set,
        .dstBinding = 0,
        .dstArrayElement = slot,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
        .pImageInfo = &descriptor_image_info,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);
}

Texture *VulkanGraphicsDevice::create_texture_impl(const TextureDescriptor &desc)
{
    return new VulkanTexture(desc, m_allocator);
}

void VulkanGraphicsDevice::destroy_texture_impl(Texture *texture) { HE_PANIC(); }

TextureView *VulkanGraphicsDevice::create_texture_view_impl(const TextureViewDescriptor &desc)
{
    return new VulkanTextureView(desc, m_device);
}

void VulkanGraphicsDevice::destroy_texture_view_impl(TextureView *texture_view) { HE_PANIC(); }

void VulkanGraphicsDevice::bind_sampled_texture_view_impl(const TextureView *texture_view, const u32 slot) const
{
    const VulkanTextureView *vulkan_texture_view = static_cast<const VulkanTextureView *>(texture_view);

    const VkDescriptorImageInfo descriptor_image_info = {
        .sampler = VK_NULL_HANDLE,
        .imageView = vulkan_texture_view->raw(),
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    const VkWriteDescriptorSet write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_sampled_image_set,
        .dstBinding = 0,
        .dstArrayElement = slot,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .pImageInfo = &descriptor_image_info,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);
}

void VulkanGraphicsDevice::bind_storage_texture_view_impl(const TextureView *texture_view, const u32 slot) const
{
    const VulkanTextureView *vulkan_texture_view = static_cast<const VulkanTextureView *>(texture_view);

    const VkDescriptorImageInfo descriptor_image_info = {
        .sampler = VK_NULL_HANDLE,
        .imageView = vulkan_texture_view->raw(),
        .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
    };

    const VkWriteDescriptorSet write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_storage_image_set,
        .dstBinding = 0,
        .dstArrayElement = slot,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .pImageInfo = &descriptor_image_info,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device, 1, &write_descriptor_set, 0, nullptr);
}

PipelineLayout *VulkanGraphicsDevice::create_pipeline_layout_impl(const PipelineLayoutDescriptor &desc)
{
    const std::array<VkDescriptorSetLayout, 4> descriptor_set_layouts = {
        m_storage_buffer_layout,
        m_sampled_image_layout,
        m_storage_image_layout,
        m_sampler_layout,
    };

    return new VulkanPipelineLayout(desc, m_device, descriptor_set_layouts);
}

void VulkanGraphicsDevice::destroy_pipeline_layout_impl(PipelineLayout *pipeline_layout) { HE_PANIC(); }

ComputePipeline *VulkanGraphicsDevice::create_compute_pipeline_impl(const ComputePipelineDescriptor &desc)
{
    return new VulkanComputePipeline(desc, m_device);
}

void VulkanGraphicsDevice::destroy_compute_pipeline_impl(ComputePipeline *compute_pipeline) { HE_PANIC(); }

RenderPipeline *VulkanGraphicsDevice::create_render_pipeline_impl(const RenderPipelineDescriptor &desc)
{
    return new VulkanRenderPipeline(desc, m_device);
}

void VulkanGraphicsDevice::destroy_render_pipeline_impl(RenderPipeline *render_pipeline) { HE_PANIC(); }

void VulkanGraphicsDevice::create_instance()
{
    const bool validation_supported = check_validation_layer_support();

    constexpr VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "HyperEngine",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "HyperEngine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_4,
    };

    constexpr VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
        .pUserData = nullptr,
    };

    u32 required_extension_count = 0;
    const char *const *required_instance_extensions = SDL_Vulkan_GetInstanceExtensions(&required_extension_count);

    std::vector<const char *> extensions(
        required_instance_extensions,
        required_instance_extensions + required_extension_count);

    if (validation_supported) {
        extensions.push_back(s_validation_extension);
    }

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

    if (validation_supported) {
        create_info.pNext = &debug_create_info;
    }

    HE_VK_CHECK(vkCreateInstance(&create_info, nullptr, &m_instance));
    volkLoadInstance(m_instance);

    if (validation_supported) {
        HE_VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_instance, &debug_create_info, nullptr, &m_debug_messenger));
    }
}

bool VulkanGraphicsDevice::check_validation_layer_support()
{
    u32 layer_count = 0;
    HE_VK_CHECK(vkEnumerateInstanceLayerProperties(&layer_count, nullptr));

    std::vector<VkLayerProperties> layer_properties(layer_count);
    HE_VK_CHECK(vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data()));

    bool found_layer = false;
    for (const VkLayerProperties &properties : layer_properties) {
        if (strcmp(properties.layerName, s_validation_layer) == 0) {
            found_layer = true;
            break;
        }
    }

    if (!found_layer) {
        return false;
    }

    u32 extension_count = 0;
    HE_VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));

    std::vector<VkExtensionProperties> extension_properties(extension_count);
    HE_VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_properties.data()));

    bool found_extension = false;
    for (const VkExtensionProperties &properties : extension_properties) {
        if (strcmp(properties.extensionName, s_validation_extension) == 0) {
            found_extension = true;
            break;
        }
    }

    if (!found_extension) {
        return false;
    }

    return true;
}

void VulkanGraphicsDevice::choose_physical_device()
{
    u32 physical_device_count = 0;
    HE_VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &physical_device_count, nullptr));

    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    HE_VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &physical_device_count, physical_devices.data()));

    for (const VkPhysicalDevice physical_device : physical_devices) {
        u32 queue_family_properties_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_properties_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_properties_count);
        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_device,
            &queue_family_properties_count,
            queue_family_properties.data());

        std::optional<u32> queue_index = std::nullopt;
        for (u32 i = 0; i < queue_family_properties.size(); ++i) {
            const VkQueueFamilyProperties properties = queue_family_properties[i];
            const bool graphics_supported = properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
            const bool presentation_supported = SDL_Vulkan_GetPresentationSupport(m_instance, physical_device, i);

            if (graphics_supported && presentation_supported) {
                queue_index = i;
                break;
            }
        }

        if (!queue_index.has_value()) {
            continue;
        }

        u32 extension_properties_count = 0;
        HE_VK_CHECK(
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_properties_count, nullptr));

        std::vector<VkExtensionProperties> extension_properties(extension_properties_count);
        HE_VK_CHECK(vkEnumerateDeviceExtensionProperties(
            physical_device,
            nullptr,
            &extension_properties_count,
            extension_properties.data()));

        bool extension_found = false;
        for (const VkExtensionProperties &properties : extension_properties) {
            if (strcmp(properties.extensionName, s_swapchain_extension) == 0) {
                extension_found = true;
                break;
            }
        }

        if (!extension_found) {
            continue;
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

        vkGetPhysicalDeviceFeatures2(physical_device, &features);

        const std::array<VkBool32, 16> features_supported = {
            features.features.sampleRateShading,
            features_12.shaderUniformBufferArrayNonUniformIndexing,
            features_12.shaderSampledImageArrayNonUniformIndexing,
            features_12.shaderStorageBufferArrayNonUniformIndexing,
            features_12.shaderStorageImageArrayNonUniformIndexing,
            features_12.descriptorBindingUniformBufferUpdateAfterBind,
            features_12.descriptorBindingSampledImageUpdateAfterBind,
            features_12.descriptorBindingStorageImageUpdateAfterBind,
            features_12.descriptorBindingStorageBufferUpdateAfterBind,
            features_12.descriptorBindingUpdateUnusedWhilePending,
            features_12.descriptorBindingPartiallyBound,
            features_12.descriptorBindingVariableDescriptorCount,
            features_12.runtimeDescriptorArray,
            features_12.timelineSemaphore,
            features_13.synchronization2,
            features_13.dynamicRendering,
        };

        bool all_supported = true;
        for (const VkBool32 supported : features_supported) {
            if (supported == VK_FALSE) {
                all_supported = false;
                break;
            }
        }

        if (!all_supported) {
            continue;
        }

        VkPhysicalDeviceProperties device_properties = { };
        vkGetPhysicalDeviceProperties(physical_device, &device_properties);

        if (device_properties.limits.timestampPeriod == 0.0f) {
            continue;
        }

        if (device_properties.limits.timestampComputeAndGraphics == VK_FALSE) {
            continue;
        }

        // NOTE: This picks the first suitable device, maybe add selection for physical device
        m_physical_device = physical_device;
        m_queue_family = queue_index.value();
        break;
    }

    HE_ASSERT(m_physical_device != VK_NULL_HANDLE);
}

void VulkanGraphicsDevice::create_logical_device()
{
    constexpr f32 queue_priority = 1.0f;
    const VkDeviceQueueCreateInfo queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = m_queue_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };

    VkPhysicalDeviceVulkan13Features features_13 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = nullptr,
        .synchronization2 = VK_TRUE,
        .dynamicRendering = VK_TRUE,
    };

    VkPhysicalDeviceVulkan12Features features_12 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &features_13,
        .shaderUniformBufferArrayNonUniformIndexing = VK_TRUE,
        .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
        .shaderStorageBufferArrayNonUniformIndexing = VK_TRUE,
        .shaderStorageImageArrayNonUniformIndexing = VK_TRUE,
        .descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE,
        .descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
        .descriptorBindingStorageImageUpdateAfterBind = VK_TRUE,
        .descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE,
        .descriptorBindingUpdateUnusedWhilePending = VK_TRUE,
        .descriptorBindingPartiallyBound = VK_TRUE,
        .descriptorBindingVariableDescriptorCount = VK_TRUE,
        .runtimeDescriptorArray = VK_TRUE,
        .timelineSemaphore = VK_TRUE,
    };

    VkPhysicalDeviceFeatures2 features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &features_12,
        .features = {
            .sampleRateShading = VK_TRUE,
        },
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

    HE_VK_CHECK(vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device));
    volkLoadDevice(m_device);

    vkGetDeviceQueue(m_device, m_queue_family, 0, &m_queue);
}

void VulkanGraphicsDevice::create_allocator()
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

    HE_VK_CHECK(vmaCreateAllocator(&allocator_create_info, &m_allocator));
}

void VulkanGraphicsDevice::create_surface(const Window &window)
{
    if (!SDL_Vulkan_CreateSurface(window.native_handle(), m_instance, nullptr, &m_surface)) {
        HE_PANIC("Failed to create vulkan surface: {}", SDL_GetError());
    }
}

void VulkanGraphicsDevice::create_swapchain(const Window &window)
{
    const VkExtent2D extent = choose_extent(window);
    const VkSurfaceFormatKHR surface_format = choose_surface_format();
    const VkPresentModeKHR present_mode = choose_present_mode();

    VkSurfaceCapabilitiesKHR capabilities = { };
    HE_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface, &capabilities));

    u32 min_image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && min_image_count > capabilities.maxImageCount) {
        min_image_count = capabilities.maxImageCount;
    }

    const VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = m_surface,
        .minImageCount = min_image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = true,
        .oldSwapchain = m_swapchain,
    };

    HE_VK_CHECK(vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swapchain));

    u32 image_count = 0;
    HE_VK_CHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, nullptr));

    std::vector<VkImage> images(image_count);
    HE_VK_CHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, images.data()));

    std::vector<VkImageView> image_views;
    for (const VkImage image : images) {
        constexpr VkComponentMapping component_mapping = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        };

        constexpr VkImageSubresourceRange subresource_range = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        const VkImageViewCreateInfo image_view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surface_format.format,
            .components = component_mapping,
            .subresourceRange = subresource_range,
        };

        VkImageView image_view = VK_NULL_HANDLE;
        HE_VK_CHECK(vkCreateImageView(m_device, &image_view_create_info, nullptr, &image_view));

        image_views.push_back(image_view);
    }

    m_swapchain_images = std::move(images);
    m_swapchain_image_views = std::move(image_views);
}

VkExtent2D VulkanGraphicsDevice::choose_extent(const Window &window) const
{
    VkSurfaceCapabilitiesKHR capabilities = { };
    HE_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface, &capabilities));

    VkExtent2D current_extent = {
        .width = capabilities.currentExtent.width,
        .height = capabilities.currentExtent.height,
    };

    const auto [width, height] = window.size();
    if (current_extent.width == std::numeric_limits<uint32_t>::max()) {
        current_extent.width = std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    }

    if (current_extent.height == std::numeric_limits<uint32_t>::max()) {
        current_extent.height
            = std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    return current_extent;
}

VkSurfaceFormatKHR VulkanGraphicsDevice::choose_surface_format() const
{
    u32 surface_format_count = 0;
    HE_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &surface_format_count, nullptr));

    std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
    HE_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        m_physical_device,
        m_surface,
        &surface_format_count,
        surface_formats.data()));

    for (const VkSurfaceFormatKHR surface_format : surface_formats) {
        if (surface_format.format == VK_FORMAT_B8G8R8A8_SRGB
            && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return surface_format;
        }
    }

    return surface_formats[0];
}

VkPresentModeKHR VulkanGraphicsDevice::choose_present_mode() const
{
    u32 present_mode_count = 0;
    HE_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &present_mode_count, nullptr));

    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    HE_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        m_physical_device,
        m_surface,
        &present_mode_count,
        present_modes.data()));

    for (const VkPresentModeKHR present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }

    return present_modes[0];
}

struct DescriptorBinding {
    VkDescriptorType type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    u32 descriptor_count = 0;
    VkDescriptorSetLayout *layout = nullptr;
    VkDescriptorSet *set = nullptr;
};

void VulkanGraphicsDevice::create_descriptors()
{
    static constexpr u32 s_requested_descriptor_count = 1000;

    std::array<DescriptorBinding, 4> bindings = {
        DescriptorBinding {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptor_count = s_requested_descriptor_count,
            .layout = &m_storage_buffer_layout,
            .set = &m_storage_buffer_set,
        },
        DescriptorBinding {
            .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptor_count = s_requested_descriptor_count,
            .layout = &m_sampled_image_layout,
            .set = &m_sampled_image_set,
        },
        DescriptorBinding {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .descriptor_count = s_requested_descriptor_count,
            .layout = &m_storage_image_layout,
            .set = &m_storage_image_set,
        },
        DescriptorBinding {
            .type = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptor_count = s_requested_descriptor_count,
            .layout = &m_sampler_layout,
            .set = &m_sampler_set,
        },
    };

    VkPhysicalDeviceProperties properties = { };
    vkGetPhysicalDeviceProperties(m_physical_device, &properties);

    std::array<VkDescriptorPoolSize, 4> pool_sizes = { };
    for (usize i = 0; i < bindings.size(); ++i) {
        u32 limit = 0;
        switch (bindings[i].type) {
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            limit = properties.limits.maxDescriptorSetStorageBuffers;
            break;
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            limit = properties.limits.maxDescriptorSetSampledImages;
            break;
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            limit = properties.limits.maxDescriptorSetStorageImages;
            break;
        case VK_DESCRIPTOR_TYPE_SAMPLER:
            limit = properties.limits.maxDescriptorSetSamplers;
            break;
        default:
            HE_UNREACHABLE();
        }

        HE_ASSERT(limit > 0);

        bindings[i].descriptor_count = std::min(bindings[i].descriptor_count, limit);

        pool_sizes[i] = VkDescriptorPoolSize {
            .type = bindings[i].type,
            .descriptorCount = bindings[i].descriptor_count,
        };
    }

    const VkDescriptorPoolCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets = static_cast<u32>(pool_sizes.size()),
        .poolSizeCount = static_cast<u32>(pool_sizes.size()),
        .pPoolSizes = pool_sizes.data(),
    };

    HE_VK_CHECK(vkCreateDescriptorPool(m_device, &create_info, nullptr, &m_descriptor_pool));

    for (const DescriptorBinding &binding : bindings) {
        const VkDescriptorSetLayoutBinding layout_binding = {
            .binding = 0,
            .descriptorType = binding.type,
            .descriptorCount = binding.descriptor_count,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        };

        constexpr VkDescriptorBindingFlags binding_flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
            | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

        const VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .pNext = nullptr,
            .bindingCount = 1,
            .pBindingFlags = &binding_flags,
        };

        const VkDescriptorSetLayoutCreateInfo layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = &binding_flags_info,
            .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
            .bindingCount = 1,
            .pBindings = &layout_binding,
        };

        HE_VK_CHECK(vkCreateDescriptorSetLayout(m_device, &layout_create_info, nullptr, binding.layout));

        const VkDescriptorSetVariableDescriptorCountAllocateInfo variable_count_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorSetCount = 1,
            .pDescriptorCounts = &binding.descriptor_count,
        };

        const VkDescriptorSetAllocateInfo set_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = &variable_count_info,
            .descriptorPool = m_descriptor_pool,
            .descriptorSetCount = 1,
            .pSetLayouts = binding.layout,
        };

        HE_VK_CHECK(vkAllocateDescriptorSets(m_device, &set_allocate_info, binding.set));
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanGraphicsDevice::debug_callback(
    const VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    const VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *)
{
    switch (severity) {
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
