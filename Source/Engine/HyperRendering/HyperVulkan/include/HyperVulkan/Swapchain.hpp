/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>

#include <volk.h>

#include <vector>

namespace HyperRendering::Vulkan
{
	class Context;

	class Swapchain
	{
	public:
		struct SwapchainSupportDetails
		{
			VkSurfaceCapabilitiesKHR surface_capabilities;
			std::vector<VkSurfaceFormatKHR> surface_formats;
			std::vector<VkPresentModeKHR> surface_presentation_modes;
		};

	public:
		explicit Swapchain(Context& context);

		auto initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;
		auto terminate() -> void;

		auto query_swap_chain_support(VkPhysicalDevice physical_device) const -> SwapchainSupportDetails;

		auto swap_chain() const -> VkSwapchainKHR;
		auto swap_chain_images() const -> const std::vector<VkImage>&;

	private:
		auto choose_swap_extent(const VkSurfaceCapabilitiesKHR& surface_capabilities) const -> VkExtent2D;
		auto choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_surface_formats) const -> VkSurfaceFormatKHR;
		auto choose_swap_presentation_mode(const std::vector<VkPresentModeKHR>& available_surface_presentation_modes) const -> VkPresentModeKHR;

	private:
		Context& m_context;

		VkSwapchainKHR m_swapchain{ VK_NULL_HANDLE };
		VkFormat m_swapchain_image_format{};
		VkExtent2D m_swapchain_extent{};
		std::vector<VkImage> m_swapchain_images{};
	};
} // namespace HyperRendering::Vulkan
