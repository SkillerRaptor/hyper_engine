/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <vector>

#include "hyper_rhi/surface.hpp"
#include "hyper_rhi/vulkan/vulkan_common.hpp"

struct SDL_Window;

namespace hyper_engine
{
    class VulkanGraphicsDevice;

    class VulkanSurface final : public Surface
    {
    public:
        VulkanSurface(VulkanGraphicsDevice &graphics_device, SDL_Window *window);
        ~VulkanSurface() override;

        uint32_t min_image_count() const override;
        uint32_t image_count() const override;

        Format format() const override;

        RefPtr<Texture> current_texture() const override;
        RefPtr<TextureView> current_texture_view() const override;

        void rebuild();

        VkSwapchainKHR swapchain() const;
        void set_texture_index(uint32_t texture_index);
        uint32_t texture_index() const;

    private:
        void create_surface(SDL_Window *window);
        void create_swapchain();
        void create_textures();
        void destroy();

        static VkExtent2D choose_extent(uint32_t width, uint32_t height, const VkSurfaceCapabilitiesKHR &capabilities);
        static VkSurfaceFormatKHR choose_format(const std::vector<VkSurfaceFormatKHR> &formats);
        static VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR> &present_modes);

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        uint32_t m_min_image_count = 0;
        uint32_t m_image_count = 0;
        VkFormat m_format = VK_FORMAT_UNDEFINED;

        uint32_t m_texture_index = 0;
        std::vector<RefPtr<Texture>> m_textures;
        std::vector<RefPtr<TextureView>> m_texture_views;
    };
} // namespace hyper_engine