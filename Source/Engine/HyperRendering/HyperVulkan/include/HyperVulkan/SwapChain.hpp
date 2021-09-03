/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperVulkan/Device.hpp"

#include <HyperPlatform/Window.hpp>

#include <volk.h>

#include <vector>

namespace HyperRendering::HyperVulkan
{
	class SwapChain
	{
	public:
		SwapChain(HyperPlatform::Window& t_window, Device& t_device, VkSurfaceKHR& m_surface);

		auto initialize() -> bool;
		auto destroy() -> bool;
		
		auto image_format() -> VkFormat&;
		auto extent() -> VkExtent2D&;

	private:
		auto create_swap_chain() -> bool;
		auto create_image_views() -> bool;

		auto choose_swap_extent(const VkSurfaceCapabilitiesKHR& surface_capabilities) const -> VkExtent2D;
		auto choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_surface_formats) const -> VkSurfaceFormatKHR;
		auto choose_swap_presentation_mode(const std::vector<VkPresentModeKHR>& available_surface_presentation_modes) const -> VkPresentModeKHR;

	private:
		HyperPlatform::Window& m_window;
		Device& m_device;
		VkSurfaceKHR& m_surface;

		VkSwapchainKHR m_swap_chain{ VK_NULL_HANDLE };
		VkFormat m_swap_chain_image_format{};
		VkExtent2D m_swap_chain_extent{};

		std::vector<VkImage> m_swap_chain_images{};
		std::vector<VkImageView> m_swap_chain_image_views{};
	};
} // namespace HyperRendering::HyperVulkan
