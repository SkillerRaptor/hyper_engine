/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/SwapChain.hpp"

#include <HyperCore/Logger.hpp>
#include <HyperMath/Common.hpp>

#include <GLFW/glfw3.h>

namespace HyperRendering::HyperVulkan
{
	SwapChain::SwapChain(HyperPlatform::Window& t_window, Device& t_device, VkSurfaceKHR& t_surface)
		: m_window(t_window)
		, m_device(t_device)
		, m_surface(t_surface)
	{
	}

	auto SwapChain::initialize() -> bool
	{
		if (!create_swap_chain())
		{
			HyperCore::Logger::fatal("SwapChain::initialize(): Failed to create Vulkan swap chain");
			return false;
		}

		if (!create_image_views())
		{
			HyperCore::Logger::fatal("SwapChain::initialize(): Failed to create Vulkan image views");
			return false;
		}

		return true;
	}

	auto SwapChain::destroy() -> bool
	{
		auto destroy_swap_chain = [this]() -> bool
		{
			if (m_swap_chain == VK_NULL_HANDLE)
			{
				return false;
			}

			vkDestroySwapchainKHR(m_device.device(), m_swap_chain, nullptr);
			HyperCore::Logger::debug("Vulkan swap chain was destroyed");

			return true;
		};

		auto destroy_image_views = [this]() -> bool
		{
			for (size_t i = 0; i < m_swap_chain_image_views.size(); ++i)
			{
				if (m_swap_chain_image_views[i] == VK_NULL_HANDLE)
				{
					return false;
				}
				
				vkDestroyImageView(m_device.device(), m_swap_chain_image_views[i], nullptr);
				HyperCore::Logger::debug("Vulkan image view #{} was destroyed", i);
			}
			
			return true;
		};

		if (!destroy_swap_chain())
		{
			return false;
		}

		if (!destroy_image_views())
		{
			return false;
		}

		return true;
	}

	auto SwapChain::create_swap_chain() -> bool
	{
		auto surface_capabilities = m_device.get_surface_capabilities(m_device.physical_device());
		auto surface_formats = m_device.get_surface_formats(m_device.physical_device());
		auto surface_present_modes = m_device.get_surface_present_modes(m_device.physical_device());

		auto extent = choose_swap_extent(surface_capabilities);
		auto surface_format = choose_swap_surface_format(surface_formats);
		auto surface_presentation_mode = choose_swap_presentation_mode(surface_present_modes);

		auto image_count = HyperMath::clamp(surface_capabilities.minImageCount + 1, surface_capabilities.minImageCount, surface_capabilities.maxImageCount);

		auto queue_family_indices = m_device.find_queue_families(m_device.physical_device());
		std::array<uint32_t, 2> queue_family_indices_array = { queue_family_indices.graphics_family.value(), queue_family_indices.present_family.value() };

		VkSwapchainCreateInfoKHR swapchain_create_info{};
		swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_create_info.surface = m_surface;
		swapchain_create_info.minImageCount = image_count;
		swapchain_create_info.imageFormat = surface_format.format;
		swapchain_create_info.imageColorSpace = surface_format.colorSpace;
		swapchain_create_info.imageExtent = extent;
		swapchain_create_info.imageArrayLayers = 1;
		swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_create_info.queueFamilyIndexCount = 0;
		swapchain_create_info.pQueueFamilyIndices = nullptr;
		swapchain_create_info.preTransform = surface_capabilities.currentTransform;
		swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchain_create_info.presentMode = surface_presentation_mode;
		swapchain_create_info.clipped = VK_TRUE;
		swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

		if (queue_family_indices.graphics_family != queue_family_indices.present_family)
		{
			swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchain_create_info.pQueueFamilyIndices = queue_family_indices_array.data();
			swapchain_create_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices_array.size());
		}

		if (vkCreateSwapchainKHR(m_device.device(), &swapchain_create_info, nullptr, &m_swap_chain) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("SwapChain::create_swap_chain(): Failed to create Vulkan swap chain");
			return false;
		}

		m_swap_chain_image_format = surface_format.format;
		m_swap_chain_extent = extent;

		uint32_t swapchain_image_count = 0;
		vkGetSwapchainImagesKHR(m_device.device(), m_swap_chain, &swapchain_image_count, nullptr);

		m_swap_chain_images.resize(swapchain_image_count);
		vkGetSwapchainImagesKHR(m_device.device(), m_swap_chain, &swapchain_image_count, m_swap_chain_images.data());

		HyperCore::Logger::debug("Vulkan swap chain was created");

		return true;
	}

	auto SwapChain::create_image_views() -> bool
	{
		m_swap_chain_image_views.resize(m_swap_chain_images.size());

		for (size_t i = 0; i < m_swap_chain_images.size(); ++i)
		{
			VkImageViewCreateInfo image_view_create_info{};
			image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			image_view_create_info.image = m_swap_chain_images[i];
			image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			image_view_create_info.format = m_swap_chain_image_format;
			image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_view_create_info.subresourceRange.baseArrayLayer = 0;
			image_view_create_info.subresourceRange.levelCount = 1;
			image_view_create_info.subresourceRange.baseArrayLayer = 0;
			image_view_create_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device.device(), &image_view_create_info, nullptr, &m_swap_chain_image_views[i]) != VK_SUCCESS)
			{
				HyperCore::Logger::fatal("SwapChain::create_image_views(): Failed to create Vulkan image view #{}", i);
				return false;
			}
			
			HyperCore::Logger::debug("Vulkan image view #{} was created", i);
		}

		return true;
	}

	auto SwapChain::choose_swap_extent(const VkSurfaceCapabilitiesKHR& surface_capabilities) const -> VkExtent2D
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
		glfwGetFramebufferSize(m_window.native_window(), &width, &height);

		VkExtent2D actual_extent{};
		actual_extent.width = HyperMath::clamp(static_cast<uint32_t>(width), surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
		actual_extent.height = HyperMath::clamp(static_cast<uint32_t>(height), surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height);

		return actual_extent;
	}

	auto SwapChain::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_surface_formats) const -> VkSurfaceFormatKHR
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

	auto SwapChain::choose_swap_presentation_mode(const std::vector<VkPresentModeKHR>& available_surface_presentation_modes) const -> VkPresentModeKHR
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
	
	auto SwapChain::image_format() -> VkFormat&
	{
		return m_swap_chain_image_format;
	}
	
	auto SwapChain::extent() -> VkExtent2D&
	{
		return m_swap_chain_extent;
	}
	
	auto SwapChain::images() -> std::vector<VkImage>&
	{
		return m_swap_chain_images;
	}

	auto SwapChain::image_views() -> std::vector<VkImageView>&
	{
		return m_swap_chain_image_views;
	}
} // namespace HyperRendering::HyperVulkan
