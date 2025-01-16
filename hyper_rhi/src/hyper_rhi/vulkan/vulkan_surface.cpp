/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_surface.hpp"

#include <algorithm>

#include <SDL3/SDL_vulkan.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"
#include "hyper_rhi/vulkan/vulkan_texture.hpp"
#include "hyper_rhi/vulkan/vulkan_texture_view.hpp"

namespace hyper_engine
{
    VulkanSurface::VulkanSurface(VulkanGraphicsDevice &graphics_device, SDL_Window *window)
        : m_graphics_device(graphics_device)
    {
        create_surface(window);
        create_swapchain();
        create_textures();
    }

    VulkanSurface::~VulkanSurface()
    {
        destroy();

        vkDestroySurfaceKHR(m_graphics_device.instance(), m_surface, nullptr);
    }

    void VulkanSurface::rebuild()
    {
        destroy();

        create_swapchain();
        create_textures();

        m_resized = false;
    }

    uint32_t VulkanSurface::min_image_count() const
    {
        return m_min_image_count;
    }

    uint32_t VulkanSurface::image_count() const
    {
        return m_image_count;
    }

    Format VulkanSurface::format() const
    {
        return VulkanTexture::format_to_texture_format(m_format);
    }

    std::shared_ptr<Texture> VulkanSurface::current_texture() const
    {
        return m_textures[m_texture_index];
    }

    std::shared_ptr<TextureView> VulkanSurface::current_texture_view() const
    {
        return m_texture_views[m_texture_index];
    }

    VkSwapchainKHR VulkanSurface::swapchain() const
    {
        return m_swapchain;
    }

    void VulkanSurface::set_texture_index(const uint32_t texture_index)
    {
        m_texture_index = texture_index;
    }

    uint32_t VulkanSurface::texture_index() const
    {
        return m_texture_index;
    }

    void VulkanSurface::create_surface(SDL_Window *window)
    {
        HE_ASSERT(SDL_Vulkan_CreateSurface(window, m_graphics_device.instance(), nullptr, &m_surface));
        HE_ASSERT(m_surface != VK_NULL_HANDLE);
    }

    void VulkanSurface::create_swapchain()
    {
        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        HE_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_graphics_device.physical_device(), m_surface, &surface_capabilities));

        const VkExtent2D surface_extent = VulkanSurface::choose_extent(m_width, m_height, surface_capabilities);
        m_width = surface_extent.width;
        m_height = surface_extent.height;

        uint32_t format_count = 0;
        HE_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_graphics_device.physical_device(), m_surface, &format_count, nullptr));

        std::vector<VkSurfaceFormatKHR> formats(format_count);
        HE_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_graphics_device.physical_device(), m_surface, &format_count, formats.data()));

        const VkSurfaceFormatKHR surface_format = VulkanSurface::choose_format(formats);
        m_format = surface_format.format;

        uint32_t present_mode_count = 0;
        HE_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_graphics_device.physical_device(), m_surface, &present_mode_count, nullptr));

        std::vector<VkPresentModeKHR> present_modes(present_mode_count);
        HE_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_graphics_device.physical_device(), m_surface, &present_mode_count, present_modes.data()));

        const VkPresentModeKHR surface_present_mode = VulkanSurface::choose_present_mode(present_modes);

        m_min_image_count = surface_capabilities.minImageCount + 1;
        m_image_count = m_min_image_count;
        if (surface_capabilities.maxImageCount > 0 && m_image_count > surface_capabilities.maxImageCount)
        {
            m_image_count = surface_capabilities.maxImageCount;
        }

        const VkSwapchainCreateInfoKHR swapchain_create_info = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = m_surface,
            .minImageCount = m_image_count,
            .imageFormat = m_format,
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

    void VulkanSurface::create_textures()
    {
        uint32_t image_count = 0;
        vkGetSwapchainImagesKHR(m_graphics_device.device(), m_swapchain, &image_count, nullptr);

        std::vector<VkImage> images(image_count);
        vkGetSwapchainImagesKHR(m_graphics_device.device(), m_swapchain, &image_count, images.data());

        uint32_t index = 0;
        for (const VkImage &image : images)
        {
            m_textures.push_back(m_graphics_device.create_texture_internal(
                {
                    .label = fmt::format("Swapchain #{}", index),
                    .width = m_width,
                    .height = m_height,
                    .depth = 1,
                    .array_size = 1,
                    .mip_levels = 1,
                    .format = VulkanTexture::format_to_texture_format(m_format),
                    .dimension = Dimension::Texture2D,
                    .usage = TextureUsage::RenderAttachment,
                },
                image));

            m_texture_views.push_back(m_graphics_device.create_texture_view({
                .label = fmt::format("Swapchain #{}", index),
                .texture = m_textures[index],
                .subresource_range =
                    {
                        .base_mip_level = 0,
                        .mip_level_count = 1,
                        .base_array_level = 0,
                        .array_layer_count = 1,
                    },
                .component_mapping =
                    {
                        .r = ComponentSwizzle::Identity,
                        .g = ComponentSwizzle::Identity,
                        .b = ComponentSwizzle::Identity,
                        .a = ComponentSwizzle::Identity,
                    },
            }));

            ++index;
        }
    }

    void VulkanSurface::destroy()
    {
        m_texture_views.clear();
        m_textures.clear();
        vkDestroySwapchainKHR(m_graphics_device.device(), m_swapchain, nullptr);
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
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
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
} // namespace hyper_engine