/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_rhi/surface.hpp"
#include "hyper_rhi/vulkan/vulkan_common.hpp"
#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"

namespace hyper_rhi
{
    class VulkanSurface final : public Surface
    {
    public:
        VulkanSurface(VulkanGraphicsDevice &graphics_device, const SurfaceDescriptor &descriptor);
        ~VulkanSurface() override;

    private:
        void create_surface(const hyper_platform::Window &window);

        void create_swapchain(const hyper_platform::Window &window);
        static VkExtent2D choose_extent(uint32_t width, uint32_t height, const VkSurfaceCapabilitiesKHR &capabilities);
        static VkSurfaceFormatKHR choose_format(const std::vector<VkSurfaceFormatKHR> &formats);
        static VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR> &present_modes);

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkSurfaceKHR m_surface;
        VkSwapchainKHR m_swapchain;
    };
} // namespace hyper_rhi