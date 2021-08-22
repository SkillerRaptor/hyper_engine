/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/Swapchain.hpp"

#include "HyperVulkan/Context.hpp"
#include "HyperVulkan/Device.hpp"
#include "HyperVulkan/Surface.hpp"

#include <HyperMath/Common.hpp>

#include <GLFW/glfw3.h>

#include <limits>

namespace HyperRendering::Vulkan
{
	Swapchain::Swapchain(Context& context)
		: m_context(context)
	{
	}

	auto Swapchain::initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
	{
		auto swapchain_result = create_swapchain();
		if (swapchain_result.is_error())
		{
			return swapchain_result;
		}

		auto image_views_result = create_image_views();
		if (image_views_result.is_error())
		{
			return image_views_result;
		}

		return {};
	}

	auto Swapchain::terminate() -> void
	{
		for (size_t i = m_swapchain_image_views.size() - 1; i < m_swapchain_image_views.size(); --i)
		{
			vkDestroyImageView(m_context.device()->device(), m_swapchain_image_views[i], nullptr);
			HyperCore::Logger::debug("Vulkan image view #{} was destroyed", i);
		}

		vkDestroySwapchainKHR(m_context.device()->device(), m_swapchain, nullptr);
		HyperCore::Logger::debug("Vulkan swapchain was destroyed");
	}

	auto Swapchain::create_swapchain() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
	{
		auto swapchain_support_details = query_swap_chain_support(m_context.device()->physical_device());

		auto extent = choose_swap_extent(swapchain_support_details.surface_capabilities);
		auto surface_format = choose_swap_surface_format(swapchain_support_details.surface_formats);
		auto surface_presentation_mode = choose_swap_presentation_mode(swapchain_support_details.surface_presentation_modes);

		auto image_count = HyperMath::clamp(
			swapchain_support_details.surface_capabilities.minImageCount + 1,
			swapchain_support_details.surface_capabilities.minImageCount,
			swapchain_support_details.surface_capabilities.maxImageCount);

		auto queue_family_indices = m_context.device()->find_queue_families(m_context.device()->physical_device());
		std::array<uint32_t, 2> queue_family_indices_array = { queue_family_indices.graphics_family.value(), queue_family_indices.presentation_family.value() };

		VkSwapchainCreateInfoKHR swapchain_create_info{};
		swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_create_info.surface = m_context.surface()->surface();
		swapchain_create_info.minImageCount = image_count;
		swapchain_create_info.imageFormat = surface_format.format;
		swapchain_create_info.imageColorSpace = surface_format.colorSpace;
		swapchain_create_info.imageExtent = extent;
		swapchain_create_info.imageArrayLayers = 1;
		swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_create_info.queueFamilyIndexCount = 0;
		swapchain_create_info.pQueueFamilyIndices = nullptr;
		swapchain_create_info.preTransform = swapchain_support_details.surface_capabilities.currentTransform;
		swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchain_create_info.presentMode = surface_presentation_mode;
		swapchain_create_info.clipped = VK_TRUE;
		swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

		if (queue_family_indices.graphics_family != queue_family_indices.presentation_family)
		{
			swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchain_create_info.pQueueFamilyIndices = queue_family_indices_array.data();
			swapchain_create_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices_array.size());
		}

		if (vkCreateSwapchainKHR(m_context.device()->device(), &swapchain_create_info, nullptr, &m_swapchain) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("Failed to create Vulkan swapchain");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

		m_swapchain_image_format = surface_format.format;
		m_swapchain_extent = extent;

		uint32_t swapchain_image_count = 0;
		vkGetSwapchainImagesKHR(m_context.device()->device(), m_swapchain, &swapchain_image_count, nullptr);

		m_swapchain_images.resize(swapchain_image_count);
		vkGetSwapchainImagesKHR(m_context.device()->device(), m_swapchain, &swapchain_image_count, m_swapchain_images.data());

		HyperCore::Logger::debug("Vulkan swapchain was created");

		return {};
	}

	auto Swapchain::create_image_views() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
	{
		m_swapchain_image_views.resize(m_swapchain_images.size());

		for (size_t i = 0; i < m_swapchain_images.size(); ++i)
		{
			VkImageViewCreateInfo image_view_create_info{};
			image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			image_view_create_info.image = m_swapchain_images[i];
			image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			image_view_create_info.format = m_swapchain_image_format;
			image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_view_create_info.subresourceRange.baseMipLevel = 0;
			image_view_create_info.subresourceRange.levelCount = 1;
			image_view_create_info.subresourceRange.baseArrayLayer = 0;
			image_view_create_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_context.device()->device(), &image_view_create_info, nullptr, &m_swapchain_image_views[i]) != VK_SUCCESS)
			{
				HyperCore::Logger::fatal("Failed to create Vulkan image view #{}", i);
				return HyperCore::Errors::ConstructError::Incomplete;
			}

			HyperCore::Logger::debug("Vulkan image view #{} was created", i);
		}

		return {};
	}

	auto Swapchain::query_swap_chain_support(VkPhysicalDevice physical_device) const -> Swapchain::SwapchainSupportDetails
	{
		Swapchain::SwapchainSupportDetails swapchain_support_details{};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			physical_device,
			m_context.surface()->surface(),
			&swapchain_support_details.surface_capabilities);

		uint32_t surface_format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			physical_device,
			m_context.surface()->surface(),
			&surface_format_count,
			nullptr);

		if (surface_format_count != 0)
		{
			swapchain_support_details.surface_formats.resize(surface_format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(
				physical_device,
				m_context.surface()->surface(),
				&surface_format_count,
				swapchain_support_details.surface_formats.data());
		}

		uint32_t surface_presentation_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			physical_device,
			m_context.surface()->surface(),
			&surface_presentation_mode_count,
			nullptr);

		if (surface_presentation_mode_count != 0)
		{
			swapchain_support_details.surface_presentation_modes.resize(surface_presentation_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				physical_device,
				m_context.surface()->surface(),
				&surface_presentation_mode_count,
				swapchain_support_details.surface_presentation_modes.data());
		}

		return swapchain_support_details;
	}

	auto Swapchain::choose_swap_extent(const VkSurfaceCapabilitiesKHR& surface_capabilities) const -> VkExtent2D
	{
		if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return surface_capabilities.currentExtent;
		}

		if (surface_capabilities.currentExtent.height != std::numeric_limits<uint32_t>::max())
		{
			return surface_capabilities.currentExtent;
		}

		int width;
		int height;
		glfwGetFramebufferSize(m_context.window().native_window(), &width, &height);

		VkExtent2D actual_extent{};
		actual_extent.width = HyperMath::clamp(static_cast<uint32_t>(width), surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
		actual_extent.height = HyperMath::clamp(static_cast<uint32_t>(height), surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height);

		return actual_extent;
	}

	auto Swapchain::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_surface_formats) const -> VkSurfaceFormatKHR
	{
		for (const auto& available_surface_format : available_surface_formats)
		{
			if (available_surface_format.format != VK_FORMAT_B8G8R8A8_SRGB)
			{
				continue;
			}

			if (available_surface_format.colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				continue;
			}

			return available_surface_format;
		}

		return available_surface_formats[0];
	}

	auto Swapchain::choose_swap_presentation_mode(const std::vector<VkPresentModeKHR>& available_surface_presentation_modes) const -> VkPresentModeKHR
	{
		for (const auto& available_surface_presentation_mode : available_surface_presentation_modes)
		{
			if (available_surface_presentation_mode != VK_PRESENT_MODE_MAILBOX_KHR)
			{
				continue;
			}

			return available_surface_presentation_mode;
		}

		return available_surface_presentation_modes[0];
	}

	auto Swapchain::swap_chain() const -> VkSwapchainKHR
	{
		return m_swapchain;
	}

	auto Swapchain::swap_chain_images() const -> const std::vector<VkImage>&
	{
		return m_swapchain_images;
	}
} // namespace HyperRendering::Vulkan
