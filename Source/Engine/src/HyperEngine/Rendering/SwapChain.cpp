/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/SwapChain.hpp"

#include "HyperEngine/Platform/Window.hpp"
#include "HyperEngine/Rendering/Device.hpp"

#include <algorithm>
#include <volk.h>

namespace HyperEngine
{
	SwapChain::SwapChain(
		const NonNullOwnPtr<Device> &device,
		const Window &window)
		: m_device(device)
		, m_window(&window)
	{
	}

	SwapChain::SwapChain(SwapChain &&other) noexcept
		: m_device(std::exchange(other.m_device, nullptr))
		, m_window(std::exchange(other.m_window, nullptr))
		, m_swap_chain(std::exchange(other.m_swap_chain, nullptr))
		, m_swap_chain_images(std::move(other.m_swap_chain_images))
		, m_swap_chain_image_views(std::move(other.m_swap_chain_image_views))
		, m_swap_chain_format(other.m_swap_chain_format)
		, m_swap_chain_extent(other.m_swap_chain_extent)
	{
	}

	SwapChain &SwapChain::operator=(SwapChain &&other) noexcept
	{
		m_device = std::exchange(other.m_device, nullptr);
		m_window = std::exchange(other.m_window, nullptr);
		m_swap_chain = std::exchange(other.m_swap_chain, nullptr);
		m_swap_chain_images = std::move(other.m_swap_chain_images);
		m_swap_chain_image_views = std::move(other.m_swap_chain_image_views);
		m_swap_chain_format = other.m_swap_chain_format;
		m_swap_chain_extent = other.m_swap_chain_extent;
		return *this;
	}

	Expected<void> SwapChain::initialize(
		const NonNullOwnPtr<VkSurfaceKHR> &surface)
	{
		const Device::SwapChainSupportDetails support_details =
			m_device->query_swap_chain_support(m_device->physical_device());
		const VkSurfaceFormatKHR surface_format =
			choose_surface_format(support_details.formats);
		const VkPresentModeKHR present_mode =
			choose_present_mode(support_details.present_modes);
		const VkExtent2D extent = choose_extent(support_details.capabilities);
		const uint32_t image_count = std::min(
			support_details.capabilities.minImageCount + 1,
			support_details.capabilities.maxImageCount);
		const Device::QueueFamilies queue_families =
			m_device->find_queue_families(m_device->physical_device());
		const std::array<uint32_t, 2> queue_family = {
			queue_families.graphics_family.value(),
			queue_families.present_family.value()
		};
		const VkSharingMode sharing_mode =
			queue_families.graphics_family != queue_families.present_family
				? VK_SHARING_MODE_CONCURRENT
				: VK_SHARING_MODE_EXCLUSIVE;
		const size_t queue_family_index_count =
			queue_families.graphics_family != queue_families.present_family
				? queue_family.size()
				: 0;
		const uint32_t *queue_family_indices =
			queue_families.graphics_family != queue_families.present_family
				? queue_family.data()
				: nullptr;
		const VkSwapchainCreateInfoKHR swapchain_create_info = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = surface,
			.minImageCount = image_count,
			.imageFormat = surface_format.format,
			.imageColorSpace = surface_format.colorSpace,
			.imageExtent = extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.imageSharingMode = sharing_mode,
			.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_index_count),
			.pQueueFamilyIndices = queue_family_indices,
			.preTransform = support_details.capabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = present_mode,
			.clipped = VK_TRUE,
			.oldSwapchain = nullptr,
		};

		VkSwapchainKHR swap_chain = nullptr;
		const auto swap_chain_result = vkCreateSwapchainKHR(
			m_device->device(), &swapchain_create_info, nullptr, &swap_chain);
		if (swap_chain_result != VK_SUCCESS)
		{
			return Error("failed to create swap chain");
		}

		m_swap_chain = NonNullOwnPtr<VkSwapchainKHR>(
			swap_chain,
			[this](VkSwapchainKHR handle)
			{
				vkDestroySwapchainKHR(m_device->device(), handle, nullptr);
			});

		m_swap_chain_format = surface_format.format;
		m_swap_chain_extent = extent;

		uint32_t swap_chain_image_count = 0;
		vkGetSwapchainImagesKHR(
			m_device->device(), m_swap_chain, &swap_chain_image_count, nullptr);

		std::vector<VkImage> swap_chain_images(swap_chain_image_count);
		vkGetSwapchainImagesKHR(
			m_device->device(),
			m_swap_chain,
			&swap_chain_image_count,
			swap_chain_images.data());

		for (VkImage &image : swap_chain_images)
		{
			auto ptr = NonNullOwnPtr<VkImage>(
				image,
				[this](VkImage handle)
				{
					vkDestroyImage(m_device->device(), handle, nullptr);
				});
			m_swap_chain_images.emplace_back(std::move(ptr));
		}

		m_swap_chain_image_views.resize(swap_chain_image_count);
		for (size_t i = 0; i < m_swap_chain_images.size(); ++i)
		{
			const VkImageViewCreateInfo image_view_create_info = {
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.image = m_swap_chain_images[i],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = m_swap_chain_format,
				.components = {
					.r = VK_COMPONENT_SWIZZLE_IDENTITY,
					.g = VK_COMPONENT_SWIZZLE_IDENTITY,
					.b = VK_COMPONENT_SWIZZLE_IDENTITY,
					.a = VK_COMPONENT_SWIZZLE_IDENTITY,
				},
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
			};

			const auto image_view_result = vkCreateImageView(
				m_device->device(),
				&image_view_create_info,
				nullptr,
				&m_swap_chain_image_views[i]);
			if (image_view_result != VK_SUCCESS)
			{
				return Error("failed to create image view #" + std::to_string(i));
			}
		}

		return {};
	}

	VkExtent2D SwapChain::choose_extent(
		const VkSurfaceCapabilitiesKHR &surface_capabilities) const
	{
		if (
			surface_capabilities.currentExtent.width != UINT32_MAX ||
			surface_capabilities.currentExtent.height != UINT32_MAX)
		{
			return surface_capabilities.currentExtent;
		}

		const Vec2ui framebuffer_size = m_window->framebuffer_size();
		const VkExtent2D extent = {
			.width = std::clamp(
				framebuffer_size.x,
				surface_capabilities.minImageExtent.width,
				surface_capabilities.maxImageExtent.width),
			.height = std::clamp(
				framebuffer_size.y,
				surface_capabilities.minImageExtent.height,
				surface_capabilities.maxImageExtent.height),
		};

		return extent;
	}

	VkSurfaceFormatKHR SwapChain::choose_surface_format(
		const std::vector<VkSurfaceFormatKHR> &surface_formats) const
	{
		for (const VkSurfaceFormatKHR &surface_format : surface_formats)
		{
			if (
				surface_format.format != VK_FORMAT_B8G8R8A8_SRGB ||
				surface_format.colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				continue;
			}

			return surface_format;
		}

		return surface_formats[0];
	}

	VkPresentModeKHR SwapChain::choose_present_mode(
		const std::vector<VkPresentModeKHR> &present_modes) const
	{
		for (const VkPresentModeKHR &present_mode : present_modes)
		{
			if (present_mode != VK_PRESENT_MODE_MAILBOX_KHR)
			{
				continue;
			}

			return present_mode;
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	Expected<NonNullOwnPtr<SwapChain>> SwapChain::create(
		const NonNullOwnPtr<VkSurfaceKHR> &surface,
		const NonNullOwnPtr<Device> &device,
		const Window &window)
	{
		assert(surface != nullptr && "The surface can't be null");

		auto swap_chain = make_non_null_own<SwapChain>(device, window);
		const auto result = swap_chain->initialize(surface);
		if (result.is_error())
		{
			return result.error();
		}

		return swap_chain;
	}
} // namespace HyperEngine
