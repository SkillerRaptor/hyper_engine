/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/Swapchain.hpp"

#include "HyperEngine/Core/Logger.hpp"
#include "HyperEngine/Math/Common.hpp"

#include <GLFW/glfw3.h>

#include <array>

namespace HyperEngine::Vulkan
{
	auto CSwapchain::create(const SDescription& description) -> bool
	{
		if (description.device == VK_NULL_HANDLE)
		{
			CLogger::fatal("CSwapchain::create(): The description vulkan device is null");
			return false;
		}

		if (description.surface == VK_NULL_HANDLE)
		{
			CLogger::fatal("CSwapchain::create(): The description vulkan surface is null");
			return false;
		}

		if (description.window == nullptr)
		{
			CLogger::fatal("CSwapchain::create(): The description window is null");
			return false;
		}

		m_device = description.device;
		m_surface = description.surface;
		m_window = description.window;

		m_swapchain_support_details = description.swapchain_support_details;
		m_queue_families = description.queue_families;

		if (!create_swapchain())
		{
			CLogger::fatal("CSwapchain::create(): Failed to create vulkan swapchain");
			return false;
		}

		if (!create_image_views())
		{
			CLogger::fatal("CSwapchain::create(): Failed to create vulkan image views");
			return false;
		}

		return true;
	}

	auto CSwapchain::create_swapchain() -> bool
	{
		const CDevice::SSwapchainSupportDetails& swapchain_support_details = m_swapchain_support_details;

		const VkSurfaceCapabilitiesKHR& surface_capabilities = swapchain_support_details.surface_capabilities;
		const VkExtent2D extent = choose_extent(surface_capabilities);

		const std::vector<VkSurfaceFormatKHR>& surface_formats = swapchain_support_details.surface_formats;
		const VkSurfaceFormatKHR surface_format = choose_surface_format(surface_formats);

		const std::vector<VkPresentModeKHR>& present_modes = swapchain_support_details.present_modes;
		const VkPresentModeKHR present_mode = choose_present_mode(present_modes);

		const uint32_t min_image_count = min(surface_capabilities.minImageCount + 1, surface_capabilities.maxImageCount);

		const CDevice::SQueueFamilies& queue_families = m_queue_families;

		const bool unique_families = queue_families.graphics_family.value() != queue_families.present_family.value();
		const std::array<uint32_t, 2> queue_families_array = {
			queue_families.graphics_family.value(),
			queue_families.present_family.value()
		};

		VkSwapchainCreateInfoKHR swapchain_create_info{};
		swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_create_info.pNext = nullptr;
		swapchain_create_info.flags = 0;
		swapchain_create_info.surface = m_surface;
		swapchain_create_info.minImageCount = min_image_count;
		swapchain_create_info.imageFormat = surface_format.format;
		swapchain_create_info.imageColorSpace = surface_format.colorSpace;
		swapchain_create_info.imageExtent = extent;
		swapchain_create_info.imageArrayLayers = 1;
		swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchain_create_info.imageSharingMode = unique_families ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
		swapchain_create_info.queueFamilyIndexCount = static_cast<uint32_t>(unique_families ? 2 : 0);
		swapchain_create_info.pQueueFamilyIndices = unique_families ? queue_families_array.data() : nullptr;
		swapchain_create_info.preTransform = surface_capabilities.currentTransform;
		swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchain_create_info.presentMode = present_mode;
		swapchain_create_info.clipped = VK_TRUE;
		swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(m_device, &swapchain_create_info, nullptr, &m_swapchain) != VK_SUCCESS)
		{
			CLogger::fatal("CSwapchain::create_swapchain(): Failed to create vulkan swapchain");
			return false;
		}

		m_image_format = surface_format.format;
		m_extent = extent;

		uint32_t image_count = 0;
		vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, nullptr);

		m_images.resize(image_count);
		vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, m_images.data());

		return true;
	}

	auto CSwapchain::create_image_views() -> bool
	{
		m_image_views.resize(m_images.size());

		for (size_t i = 0; i < m_images.size(); ++i)
		{
			VkImageViewCreateInfo image_view_create_info{};
			image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			image_view_create_info.pNext = nullptr;
			image_view_create_info.flags = 0;
			image_view_create_info.image = m_images[i];
			image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			image_view_create_info.format = m_image_format;
			image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_view_create_info.subresourceRange.baseMipLevel = 0;
			image_view_create_info.subresourceRange.levelCount = 1;
			image_view_create_info.subresourceRange.baseArrayLayer = 0;
			image_view_create_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device, &image_view_create_info, nullptr, &m_image_views[i]) != VK_SUCCESS)
			{
				CLogger::fatal("CSwapchain::create_swapchain(): Failed to create vulkan image view #{}", i);
				return false;
			}
		}

		return true;
	}

	auto CSwapchain::destroy() -> void
	{
		for (VkImageView& image_view : m_image_views)
		{
			if (image_view != VK_NULL_HANDLE)
			{
				vkDestroyImageView(m_device, image_view, nullptr);
			}
		}

		if (m_swapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
		}
	}

	auto CSwapchain::choose_extent(const VkSurfaceCapabilitiesKHR& surface_capabilities) const -> VkExtent2D
	{
		if (surface_capabilities.currentExtent.width != UINT32_MAX || surface_capabilities.currentExtent.height != UINT32_MAX)
		{
			return surface_capabilities.currentExtent;
		}

		int32_t width = 0;
		int32_t height = 0;
		glfwGetFramebufferSize(m_window, &width, &height);

		VkExtent2D extent{};
		extent.width = clamp(static_cast<uint32_t>(width), surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
		extent.height = clamp(static_cast<uint32_t>(height), surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height);

		return extent;
	}

	auto CSwapchain::choose_surface_format(const std::vector<VkSurfaceFormatKHR>& available_surface_formats) const -> VkSurfaceFormatKHR
	{
		for (const VkSurfaceFormatKHR& surface_format : available_surface_formats)
		{
			if (surface_format.format == VK_FORMAT_B8G8R8A8_SRGB && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return surface_format;
			}
		}

		return available_surface_formats[0];
	}

	auto CSwapchain::choose_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) const -> VkPresentModeKHR
	{
		for (const VkPresentModeKHR& present_mode : available_present_modes)
		{
			if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return present_mode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	auto CSwapchain::image_format() const noexcept -> VkFormat
	{
		return m_image_format;
	}

	auto CSwapchain::extent() const noexcept -> VkExtent2D
	{
		return m_extent;
	}
	auto CSwapchain::images() const -> std::vector<VkImage>
	{
		return m_images;
	}

	auto CSwapchain::image_views() const -> std::vector<VkImageView>
	{
		return m_image_views;
	}
} // namespace HyperEngine::Vulkan
