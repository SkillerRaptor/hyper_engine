/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_surface.hpp"

#include <algorithm>

#include <GLFW/glfw3.h>

namespace hyper_rhi
{
    VulkanSurface::VulkanSurface(VulkanGraphicsDevice &graphics_device, const SurfaceDescriptor &descriptor)
        : m_graphics_device(graphics_device)
        , m_surface(VK_NULL_HANDLE)
        , m_swapchain(VK_NULL_HANDLE)
        , m_current_texture_index(0)
        , m_resized(false)
        , m_width(descriptor.window.width())
        , m_height(descriptor.window.height())
    {
        this->create_surface(descriptor.window);
        this->create_swapchain();

        // TODO: Retrieve swapchain images
    }

    VulkanSurface::~VulkanSurface()
    {
        this->destroy();

        vkDestroySurfaceKHR(m_graphics_device.instance(), m_surface, nullptr);
    }

    void VulkanSurface::rebuild()
    {
        this->destroy();

        this->create_swapchain();

        // TODO: Retrieve swapchain images

        m_resized = false;
    }

    VkSwapchainKHR VulkanSurface::swapchain() const
    {
        return m_swapchain;
    }

    void VulkanSurface::set_current_texture_index(const uint32_t current_texture_index)
    {
        m_current_texture_index = current_texture_index;
    }

    uint32_t VulkanSurface::current_texture_index() const
    {
        return m_current_texture_index;
    }

    bool VulkanSurface::resized() const
    {
        return m_resized;
    }

    void VulkanSurface::resize(const uint32_t width, const uint32_t height)
    {
        m_resized = true;
        m_width = width;
        m_height = height;
    }

    TextureHandle VulkanSurface::current_texture() const
    {
        HE_UNREACHABLE();
    }

    void VulkanSurface::create_surface(const hyper_platform::Window &window)
    {
        HE_VK_CHECK(glfwCreateWindowSurface(m_graphics_device.instance(), window.native_window(), nullptr, &m_surface));
        HE_ASSERT(m_surface != VK_NULL_HANDLE);
    }

    void VulkanSurface::create_swapchain()
    {
        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        HE_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_graphics_device.physical_device(), m_surface, &surface_capabilities));

        const VkExtent2D surface_extent = VulkanSurface::choose_extent(m_width, m_height, surface_capabilities);

        uint32_t format_count = 0;
        HE_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_graphics_device.physical_device(), m_surface, &format_count, nullptr));

        std::vector<VkSurfaceFormatKHR> formats(format_count);
        HE_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_graphics_device.physical_device(), m_surface, &format_count, formats.data()));

        const VkSurfaceFormatKHR surface_format = VulkanSurface::choose_format(formats);

        uint32_t present_mode_count = 0;
        HE_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_graphics_device.physical_device(), m_surface, &present_mode_count, nullptr));

        std::vector<VkPresentModeKHR> present_modes(present_mode_count);
        HE_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_graphics_device.physical_device(), m_surface, &present_mode_count, present_modes.data()));

        const VkPresentModeKHR surface_present_mode = VulkanSurface::choose_present_mode(present_modes);

        uint32_t image_count = surface_capabilities.minImageCount + 1;
        if (surface_capabilities.maxImageCount > 0 && image_count > surface_capabilities.maxImageCount)
        {
            image_count = surface_capabilities.maxImageCount;
        }

        const VkSwapchainCreateInfoKHR swapchain_create_info = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = m_surface,
            .minImageCount = image_count,
            .imageFormat = surface_format.format,
            .imageColorSpace = surface_format.colorSpace,
            .imageExtent = surface_extent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = surface_capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = surface_present_mode,
            .clipped = true,
            .oldSwapchain = VK_NULL_HANDLE,
        };

        HE_VK_CHECK(vkCreateSwapchainKHR(m_graphics_device.device(), &swapchain_create_info, nullptr, &m_swapchain));
        HE_ASSERT(m_swapchain != VK_NULL_HANDLE);
    }

    VkExtent2D VulkanSurface::choose_extent(const uint32_t width, const uint32_t height, const VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() ||
            capabilities.currentExtent.height != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }

        const VkExtent2D extent = {
            .width = std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            .height = std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
        };

        return extent;
    }

    VkSurfaceFormatKHR VulkanSurface::choose_format(const std::vector<VkSurfaceFormatKHR> &formats)
    {
        for (const VkSurfaceFormatKHR &format : formats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return format;
            }
        }

        return formats[0];
    }

    VkPresentModeKHR VulkanSurface::choose_present_mode(const std::vector<VkPresentModeKHR> &present_modes)
    {
        for (const VkPresentModeKHR &present_mode : present_modes)
        {
            if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return present_mode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    void VulkanSurface::destroy()
    {
        vkDestroySwapchainKHR(m_graphics_device.device(), m_swapchain, nullptr);
    }
} // namespace hyper_rhi