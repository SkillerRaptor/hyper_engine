/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"

#include <array>
#include <map>
#include <set>
#include <vector>

#include <SDL3/SDL_vulkan.h>

#define VMA_IMPLEMENTATION
#include "hyper_rhi/descriptor_manager.hpp"

#include <vk_mem_alloc.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

#include "hyper_rhi/vulkan/vulkan_buffer.hpp"
#include "hyper_rhi/vulkan/vulkan_command_list.hpp"
#include "hyper_rhi/vulkan/vulkan_descriptor_manager.hpp"
#include "hyper_rhi/vulkan/vulkan_surface.hpp"

namespace hyper_engine
{
    static constexpr std::array<const char *, 1> g_validation_layers = {
        "VK_LAYER_KHRONOS_validation",
    };

    static constexpr std::array<const char *, 1> g_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    VulkanGraphicsDevice::VulkanGraphicsDevice(const GraphicsDeviceDescriptor &descriptor)
        : m_graphics_api(descriptor.graphics_api)
        , m_debug_validation(descriptor.debug_validation)
        , m_debug_label(descriptor.debug_label)
        , m_debug_marker(descriptor.debug_marker)
    {
        volkInitialize();

        if (m_debug_validation)
        {
            if (VulkanGraphicsDevice::check_validation_layer_support())
            {
                m_debug_validation = true;
            }
            else
            {
                // HE_WARN("Failed to enable requested Validation Layers");
            }
        }

        if (!m_debug_validation)
        {
            m_debug_marker = false;
            m_debug_label = false;
        }

        create_instance();
        create_debug_messenger();
        choose_physical_device();
        create_device();
        create_allocator();

        m_descriptor_manager = new VulkanDescriptorManager(*this);

        create_frames();

        // HE_INFO("Created Vulkan Graphics Device");
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        wait_for_idle();

        destroy_resources();

        for (const FrameData &frame : m_frames)
        {
            vkDestroyFence(m_device, frame.render_fence, nullptr);
            vkDestroySemaphore(m_device, frame.submit_semaphore, nullptr);
            vkDestroyCommandPool(m_device, frame.command_pool, nullptr);
        }

        delete m_descriptor_manager;

        vmaDestroyAllocator(m_allocator);

        // NOTE: What if the queue is not dropped before?
        m_queue = nullptr;

        vkDestroyDevice(m_device, nullptr);

        if (m_debug_validation)
        {
            vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        }

        vkDestroyInstance(m_instance, nullptr);
    }

    RefPtr<Surface> VulkanGraphicsDevice::create_surface(SDL_Window *window)
    {
        return make_ref<VulkanSurface>(*this, window);
    }

    RefPtr<CommandList> VulkanGraphicsDevice::create_command_list()
    {
        return make_ref<VulkanCommandList>(*this);
    }

    void VulkanGraphicsDevice::begin_marker(
        const VkCommandBuffer command_buffer,
        const MarkerType type,
        const std::string_view name,
        const LabelColor color) const
    {
        HE_ASSERT(command_buffer != VK_NULL_HANDLE);

        if (m_debug_marker && !name.empty())
        {
            const std::string_view suffix = [&type]()
            {
                switch (type)
                {
                case MarkerType::ComputePass:
                    return "Compute Pass";
                case MarkerType::RenderPass:
                    return "Render Pass";
                default:
                    HE_UNREACHABLE();
                }
            }();

            const std::string label = fmt::format("{} {}", name, suffix);

            const float r = static_cast<float>(color.red) / 255.0f;
            const float g = static_cast<float>(color.green) / 255.0f;
            const float b = static_cast<float>(color.blue) / 255.0f;

            const VkDebugUtilsLabelEXT debug_label = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
                .pNext = nullptr,
                .pLabelName = label.c_str(),
                .color =
                    {
                        r,
                        g,
                        b,
                        1.0f,
                    },
            };

            vkCmdBeginDebugUtilsLabelEXT(command_buffer, &debug_label);
        }
    }

    void VulkanGraphicsDevice::end_marker(const VkCommandBuffer command_buffer) const
    {
        HE_ASSERT(command_buffer != VK_NULL_HANDLE);

        if (m_debug_marker)
        {
            vkCmdEndDebugUtilsLabelEXT(command_buffer);
        }
    }

    void VulkanGraphicsDevice::set_object_name(const void *handle, const ObjectType type, std::string_view name) const
    {
        if (m_debug_label && handle != nullptr && !name.empty())
        {
            const VkObjectType object_type = [&type]()
            {
                switch (type)
                {
                case ObjectType::Buffer:
                    return VK_OBJECT_TYPE_BUFFER;
                case ObjectType::CommandPool:
                    return VK_OBJECT_TYPE_COMMAND_POOL;
                case ObjectType::Fence:
                    return VK_OBJECT_TYPE_FENCE;
                case ObjectType::Image:
                    return VK_OBJECT_TYPE_IMAGE;
                case ObjectType::ImageView:
                    return VK_OBJECT_TYPE_IMAGE_VIEW;
                case ObjectType::Pipeline:
                    return VK_OBJECT_TYPE_PIPELINE;
                case ObjectType::PipelineLayout:
                    return VK_OBJECT_TYPE_PIPELINE_LAYOUT;
                case ObjectType::Queue:
                    return VK_OBJECT_TYPE_QUEUE;
                case ObjectType::ShaderModule:
                    return VK_OBJECT_TYPE_SHADER_MODULE;
                case ObjectType::Semaphore:
                    return VK_OBJECT_TYPE_SEMAPHORE;
                case ObjectType::Sampler:
                    return VK_OBJECT_TYPE_SAMPLER;
                default:
                    HE_UNREACHABLE();
                }
            }();

            const VkDebugUtilsObjectNameInfoEXT debug_marker_object_name_info = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .pNext = nullptr,
                .objectType = object_type,
                .objectHandle = reinterpret_cast<uint64_t>(handle),
                .pObjectName = name.data(),
            };

            HE_VK_CHECK(vkSetDebugUtilsObjectNameEXT(m_device, &debug_marker_object_name_info));
        }
    }

    void VulkanGraphicsDevice::destroy_resources()
    {
        for (const BufferEntry &buffer_entry : m_resource_queue.buffers)
        {
            if (buffer_entry.allocation != VK_NULL_HANDLE)
            {
                vmaDestroyBuffer(m_allocator, buffer_entry.buffer, buffer_entry.allocation);
            }
            m_descriptor_manager->retire_handle(buffer_entry.handle);
        }
        m_resource_queue.buffers.clear();

        for (const VkPipeline &compute_pipeline : m_resource_queue.compute_pipelines)
        {
            vkDestroyPipeline(m_device, compute_pipeline, nullptr);
        }
        m_resource_queue.compute_pipelines.clear();

        for (const VkPipeline &graphics_pipeline : m_resource_queue.graphics_pipelines)
        {
            vkDestroyPipeline(m_device, graphics_pipeline, nullptr);
        }
        m_resource_queue.graphics_pipelines.clear();

        for (const VkPipelineLayout &pipeline_layout : m_resource_queue.pipeline_layouts)
        {
            vkDestroyPipelineLayout(m_device, pipeline_layout, nullptr);
        }
        m_resource_queue.pipeline_layouts.clear();

        for (const VkSampler &sampler : m_resource_queue.samplers)
        {
            vkDestroySampler(m_device, sampler, nullptr);
        }
        m_resource_queue.samplers.clear();

        for (const VkShaderModule &shader_module : m_resource_queue.shader_modules)
        {
            vkDestroyShaderModule(m_device, shader_module, nullptr);
        }
        m_resource_queue.shader_modules.clear();

        for (const TextureEntry &texture_entry : m_resource_queue.textures)
        {
            if (texture_entry.allocation != VK_NULL_HANDLE)
            {
                vmaDestroyImage(m_allocator, texture_entry.image, texture_entry.allocation);
            }
        }
        m_resource_queue.textures.clear();

        for (const TextureViewEntry &texture_view_entry : m_resource_queue.texture_views)
        {
            vkDestroyImageView(m_device, texture_view_entry.view, nullptr);
            m_descriptor_manager->retire_handle(texture_view_entry.handle);
        }
        m_resource_queue.texture_views.clear();
    }

    void VulkanGraphicsDevice::begin_frame(RefPtr<Surface> &surface, const uint32_t frame_index)
    {
        VulkanSurface &vulkan_surface = static_cast<VulkanSurface &>(*surface);

        m_current_frame_index = frame_index;

        const uint64_t wait_frame_index = current_frame().semaphore_counter;
        const VkSemaphoreWaitInfo semaphore_wait_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
            .pNext = nullptr,
            .flags = 0,
            .semaphoreCount = 1,
            .pSemaphores = &current_frame().submit_semaphore,
            .pValues = &wait_frame_index,
        };
        HE_VK_CHECK(vkWaitSemaphores(m_device, &semaphore_wait_info, std::numeric_limits<uint64_t>::max()));

        destroy_resources();

        if (vulkan_surface.resized())
        {
            vulkan_surface.rebuild();
        }

        uint32_t image_index = 0;
        HE_VK_CHECK(vkAcquireNextImageKHR(
            m_device,
            vulkan_surface.swapchain(),
            std::numeric_limits<uint64_t>::max(),
            VK_NULL_HANDLE,
            current_frame().render_fence,
            &image_index));

        vulkan_surface.set_texture_index(image_index);
    }

    void VulkanGraphicsDevice::end_frame() const
    {
        HE_VK_CHECK(vkWaitForFences(m_device, 1, &current_frame().render_fence, VK_TRUE, std::numeric_limits<uint64_t>::max()));
        HE_VK_CHECK(vkResetFences(m_device, 1, &current_frame().render_fence));
    }

    void VulkanGraphicsDevice::execute(const RefPtr<CommandList> &command_list)
    {
        m_frames[m_current_frame_index % GraphicsDevice::s_frame_count].semaphore_counter += 1;

        const VulkanCommandList &vulkan_command_list = static_cast<const VulkanCommandList &>(*command_list);

        const VkCommandBufferSubmitInfo command_buffer_submit_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .pNext = nullptr,
            .commandBuffer = vulkan_command_list.command_buffer(),
            .deviceMask = 0,
        };

        const VkSemaphoreSubmitInfo submit_semaphore_submit_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .pNext = nullptr,
            .semaphore = current_frame().submit_semaphore,
            .value = current_frame().semaphore_counter,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .deviceIndex = 0,
        };

        const VkSubmitInfo2 submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .pNext = nullptr,
            .flags = 0,
            .waitSemaphoreInfoCount = 0,
            .pWaitSemaphoreInfos = nullptr,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &command_buffer_submit_info,
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos = &submit_semaphore_submit_info,
        };

        HE_VK_CHECK(vkQueueSubmit2(m_queue, 1, &submit_info, VK_NULL_HANDLE));
    }

    void VulkanGraphicsDevice::present(const RefPtr<Surface> &surface) const
    {
        const VulkanSurface &vulkan_surface = static_cast<const VulkanSurface &>(*surface);

        const VkSemaphoreWaitInfo semaphore_wait_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
            .pNext = nullptr,
            .flags = 0,
            .semaphoreCount = 1,
            .pSemaphores = &current_frame().submit_semaphore,
            .pValues = &current_frame().semaphore_counter,
        };
        HE_VK_CHECK(vkWaitSemaphores(m_device, &semaphore_wait_info, std::numeric_limits<uint64_t>::max()));

        const VkSwapchainKHR swapchain = vulkan_surface.swapchain();
        const uint32_t texture_index = vulkan_surface.texture_index();

        const VkPresentInfoKHR present_info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &texture_index,
            .pResults = nullptr,
        };

        HE_VK_CHECK(vkQueuePresentKHR(m_queue, &present_info));
    }

    void VulkanGraphicsDevice::wait_for_idle() const
    {
        HE_VK_CHECK(vkDeviceWaitIdle(m_device));
    }

    GraphicsApi VulkanGraphicsDevice::graphics_api() const
    {
        return m_graphics_api;
    }

    bool VulkanGraphicsDevice::debug_validation() const
    {
        return m_debug_validation;
    }

    bool VulkanGraphicsDevice::debug_label() const
    {
        return m_debug_label;
    }

    bool VulkanGraphicsDevice::debug_marker() const
    {
        return m_debug_marker;
    }

    DescriptorManager &VulkanGraphicsDevice::descriptor_manager()
    {
        return *static_cast<DescriptorManager *>(m_descriptor_manager);
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

    uint32_t VulkanGraphicsDevice::queue_family() const
    {
        return m_queue_family;
    }

    VkQueue VulkanGraphicsDevice::queue() const
    {
        return m_queue;
    }

    VmaAllocator VulkanGraphicsDevice::allocator() const
    {
        return m_allocator;
    }

    ResourceQueue &VulkanGraphicsDevice::resource_queue()
    {
        return m_resource_queue;
    }

    const FrameData &VulkanGraphicsDevice::current_frame() const
    {
        return m_frames[m_current_frame_index % GraphicsDevice::s_frame_count];
    }

    uint32_t VulkanGraphicsDevice::current_frame_index() const
    {
        return m_current_frame_index;
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

        const void *next = m_debug_validation ? &debug_create_info : nullptr;

        constexpr VkApplicationInfo application_info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "HyperEngine",
            .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .pEngineName = "HyperEngine",
            .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .apiVersion = VK_API_VERSION_1_3,
        };

        const uint32_t layer_count = m_debug_validation ? static_cast<uint32_t>(g_validation_layers.size()) : 0;
        const char *const *layers = m_debug_validation ? g_validation_layers.data() : nullptr;

        uint32_t required_extension_count = 0;
        const char *const *required_extensions = SDL_Vulkan_GetInstanceExtensions(&required_extension_count);

        std::vector<const char *> extensions(required_extensions, required_extensions + required_extension_count);
        if (m_debug_validation)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        const VkInstanceCreateInfo instance_create_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = next,
            .flags = 0,
            .pApplicationInfo = &application_info,
            .enabledLayerCount = layer_count,
            .ppEnabledLayerNames = layers,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
        };

        HE_VK_CHECK(vkCreateInstance(&instance_create_info, nullptr, &m_instance));
        HE_ASSERT(m_instance != VK_NULL_HANDLE);

        volkLoadInstance(m_instance);
    }

    void VulkanGraphicsDevice::create_debug_messenger()
    {
        if (!m_debug_validation)
        {
            return;
        }

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

        HE_VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_instance, &debug_create_info, nullptr, &m_debug_messenger));
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
            possible_physical_devices.insert({score, physical_device});
        }

        HE_ASSERT(possible_physical_devices.rbegin()->first != 0);

        m_physical_device = possible_physical_devices.rbegin()->second;

        VkPhysicalDeviceProperties properties = {};
        vkGetPhysicalDeviceProperties(m_physical_device, &properties);

        const std::string_view device_type = [&properties]()
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
                HE_UNREACHABLE();
            }
        }();

        // FIXME: Log queues, extensions and features
        // FIXME: Log missing criteria if no device was found

        // HE_INFO("Physical Device Info:");
        // HE_INFO("  Name: {}", properties.deviceName);
        // HE_INFO(
        //     "  API Version: {}.{}.{}",
        //     VK_VERSION_MAJOR(properties.apiVersion),
        //     VK_VERSION_MINOR(properties.apiVersion),
        //     VK_VERSION_PATCH(properties.apiVersion));
        // HE_INFO("  Type: {}", device_type);
    }

    uint32_t VulkanGraphicsDevice::rate_physical_device(const VkPhysicalDevice &physical_device) const
    {
        uint32_t score = 0;

        const std::optional<uint32_t> queue_family = find_queue_family(physical_device);
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
            const bool present_supported = SDL_Vulkan_GetPresentationSupport(m_instance, physical_device, index);

            if (graphics_supported && present_supported)
            {
                return index;
            }

            ++index;
        }

        return std::nullopt;
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

        size_t feature_count = 0;
        const auto *current = static_cast<const VkBaseInStructure *>(device_features.pNext);
        while (current)
        {
            current = current->pNext;
            feature_count += 1;
        }

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

        const uint32_t layer_count = m_debug_validation ? static_cast<uint32_t>(g_validation_layers.size()) : 0;
        const char *const *layers = m_debug_validation ? g_validation_layers.data() : nullptr;

        const VkDeviceCreateInfo device_create_info = {
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

        HE_VK_CHECK(vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device));
        HE_ASSERT(m_device != VK_NULL_HANDLE);

        volkLoadDevice(m_device);

        VkQueue queue = VK_NULL_HANDLE;
        vkGetDeviceQueue(m_device, queue_family.value(), 0, &queue);

        m_queue_family = queue_family.value();
        m_queue = queue;

        // FIXME: Retrieve queue type
        set_object_name(m_queue, ObjectType::Queue, "Graphics");
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
        HE_ASSERT(m_allocator != VK_NULL_HANDLE);
    }

    void VulkanGraphicsDevice::create_frames()
    {
        for (size_t index = 0; index < GraphicsDevice::s_frame_count; ++index)
        {
            const VkCommandPoolCreateInfo command_pool_create_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = m_queue_family,
            };

            HE_VK_CHECK(vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_frames[index].command_pool));
            HE_ASSERT(m_frames[index].command_pool != VK_NULL_HANDLE);

            set_object_name(m_frames[index].command_pool, ObjectType::CommandPool, fmt::format("Frame #{}", index));

            const VkCommandBufferAllocateInfo command_buffer_allocate_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = m_frames[index].command_pool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1,
            };

            HE_VK_CHECK(vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &m_frames[index].command_buffer));
            HE_ASSERT(m_frames[index].command_buffer != VK_NULL_HANDLE);

            constexpr VkFenceCreateInfo fence_create_info = {
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
            };

            HE_VK_CHECK(vkCreateFence(m_device, &fence_create_info, nullptr, &m_frames[index].render_fence));
            HE_ASSERT(m_frames[index].render_fence != VK_NULL_HANDLE);

            set_object_name(m_frames[index].render_fence, ObjectType::Fence, fmt::format("Frame Render #{}", index));

            VkSemaphoreTypeCreateInfo submit_semaphore_type_create_info = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                .pNext = nullptr,
                .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
                .initialValue = 0,
            };

            const VkSemaphoreCreateInfo submit_semaphore_create_info = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                .pNext = &submit_semaphore_type_create_info,
                .flags = 0,
            };

            HE_VK_CHECK(vkCreateSemaphore(m_device, &submit_semaphore_create_info, nullptr, &m_frames[index].submit_semaphore));
            HE_ASSERT(m_frames[index].submit_semaphore != VK_NULL_HANDLE);

            set_object_name(m_frames[index].submit_semaphore, ObjectType::Semaphore, fmt::format("Frame Submit #{}", index));
        }
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

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanGraphicsDevice::debug_callback(
        const VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        const VkDebugUtilsMessageTypeFlagsEXT,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void *)
    {
        /*
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
        */

        return VK_FALSE;
    }
} // namespace hyper_engine
