/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/vulkan/vulkan_swapchain.h"

#include "hyper_common/assertion.h"
#include "hyper_common/logger.h"
#include "hyper_math/common.h"
#include "hyper_rendering/vulkan/vulkan_device.h"

#include <limits.h>

static VkSurfaceFormatKHR hyper_choose_surface_format(
	struct hyper_vector *formats)
{
	hyper_assert$(formats != NULL);

	hyper_vector_foreach$(*formats, const VkSurfaceFormatKHR, surface_format)
	{
		if (surface_format->format != VK_FORMAT_B8G8R8A8_SRGB)
		{
			continue;
		}

		if (surface_format->colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			continue;
		}

		return *surface_format;
	}

	return *(VkSurfaceFormatKHR *) hyper_vector_get(formats, 0);
}

static VkPresentModeKHR hyper_choose_present_mode(
	struct hyper_vector *present_modes)
{
	hyper_assert$(present_modes != NULL);

	hyper_vector_foreach$(
		*present_modes,
		const VkPresentModeKHR,
		present_mode)
	{
		if (*present_mode != VK_PRESENT_MODE_MAILBOX_KHR)
		{
			continue;
		}

		return *present_mode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D hyper_choose_extent(
	const VkSurfaceCapabilitiesKHR *capabilities,
	struct hyper_window *window)
{
	hyper_assert$(capabilities != NULL);
	hyper_assert$(window != NULL);

	if (
		capabilities->currentExtent.width != UINT32_MAX ||
		capabilities->currentExtent.height != UINT32_MAX)
	{
		return capabilities->currentExtent;
	}

	const VkExtent2D extent = {
		.width = hyper_clamp$(
			window->framebuffer_width,
			capabilities->minImageExtent.width,
			capabilities->maxImageExtent.width),
		.height = hyper_clamp$(
			window->framebuffer_height,
			capabilities->minImageExtent.height,
			capabilities->maxImageExtent.height),
	};

	return extent;
}

enum hyper_result hyper_vulkan_swapchain_create(
	struct hyper_vulkan_context *vulkan_context,
	struct hyper_window *window)
{
	hyper_assert$(vulkan_context != NULL);

	struct hyper_swapchain_details swapchain_details = { 0 };
	hyper_vulkan_swapchain_query_details(
		&swapchain_details,
		vulkan_context->physical_device,
		vulkan_context->surface);

	const VkSurfaceFormatKHR surface_format =
		hyper_choose_surface_format(&swapchain_details.formats);
	const VkPresentModeKHR present_mode =
		hyper_choose_present_mode(&swapchain_details.formats);
	const VkExtent2D extent =
		hyper_choose_extent(&swapchain_details.capabilities, window);

	const uint32_t image_count = hyper_clamp$(
		swapchain_details.capabilities.minImageCount + 1,
		swapchain_details.capabilities.minImageCount,
		swapchain_details.capabilities.maxImageCount);

	struct hyper_queue_families queue_families = { 0 };
	hyper_vulkan_device_find_queue_families(
		&queue_families, vulkan_context->physical_device, vulkan_context->surface);

	const uint32_t queue_family_indices[2] = {
		queue_families.graphics_family,
		queue_families.present_family,
	};

	const VkSharingMode sharing_mode =
		(queue_families.graphics_family == queue_families.present_family)
			? VK_SHARING_MODE_EXCLUSIVE
			: VK_SHARING_MODE_CONCURRENT;
	const uint32_t family_index_count =
		(queue_families.graphics_family == queue_families.present_family) ? 0 : 2;
	const uint32_t *family_indices =
		(queue_families.graphics_family == queue_families.present_family)
			? NULL
			: queue_family_indices;

	const VkSwapchainCreateInfoKHR swapchain_create_info = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = vulkan_context->surface,
		.minImageCount = image_count,
		.imageFormat = surface_format.format,
		.imageColorSpace = surface_format.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = sharing_mode,
		.queueFamilyIndexCount = family_index_count,
		.pQueueFamilyIndices = family_indices,
		.preTransform = swapchain_details.capabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = present_mode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE,
	};

	hyper_vulkan_swapchain_destroy_details(&swapchain_details);

	if (
		vkCreateSwapchainKHR(
			vulkan_context->device,
			&swapchain_create_info,
			NULL,
			&vulkan_context->swapchain) != VK_SUCCESS)
	{
		hyper_logger_error$("Failed to create swapchain\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	hyper_vector_create(&vulkan_context->swapchain_images, sizeof(VkImage));

	uint32_t swapchain_image_count = 0;
	vkGetSwapchainImagesKHR(
		vulkan_context->device,
		vulkan_context->swapchain,
		&swapchain_image_count,
		NULL);

	hyper_vector_resize(&vulkan_context->swapchain_images, swapchain_image_count);
	vkGetSwapchainImagesKHR(
		vulkan_context->device,
		vulkan_context->swapchain,
		&swapchain_image_count,
		vulkan_context->swapchain_images.data);

	vulkan_context->swapchain_format = surface_format.format;
	vulkan_context->swapchain_extent = extent;

	hyper_vector_create(
		&vulkan_context->swapchain_images_views, sizeof(VkImageView));
	hyper_vector_resize(
		&vulkan_context->swapchain_images_views,
		vulkan_context->swapchain_images.size);

	hyper_vector_foreach$(
		vulkan_context->swapchain_images,
		const VkImage,
		image)
	{
		const VkImageViewCreateInfo image_view_create_info = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = *image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = vulkan_context->swapchain_format,
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

		VkImageView *image_view =
			hyper_vector_get(&vulkan_context->swapchain_images_views, vector_index);
		if (
			vkCreateImageView(
				vulkan_context->device, &image_view_create_info, NULL, image_view) !=
			VK_SUCCESS)
		{
			hyper_logger_error$("Failed to create image view #%u\n", vector_index);
			return HYPER_RESULT_INITIALIZATION_FAILED;
		}
	}

	return HYPER_RESULT_SUCCESS;
}

void hyper_vulkan_swapchain_destroy(struct hyper_vulkan_context *vulkan_context)
{
	hyper_assert$(vulkan_context != NULL);

	hyper_vector_foreach$(
		vulkan_context->swapchain_images_views,
		const VkImageView,
		image_view)
	{
		vkDestroyImageView(vulkan_context->device, *image_view, NULL);
	}

	hyper_vector_destroy(&vulkan_context->swapchain_images_views);
	hyper_vector_destroy(&vulkan_context->swapchain_images);

	vkDestroySwapchainKHR(
		vulkan_context->device, vulkan_context->swapchain, NULL);
}

void hyper_vulkan_swapchain_query_details(
	struct hyper_swapchain_details *swapchain_details,
	VkPhysicalDevice physical_device,
	VkSurfaceKHR surface)
{
	hyper_assert$(swapchain_details != NULL);
	hyper_assert$(physical_device != VK_NULL_HANDLE);
	hyper_assert$(surface != VK_NULL_HANDLE);

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		physical_device, surface, &swapchain_details->capabilities);

	uint32_t format_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		physical_device, surface, &format_count, NULL);

	hyper_vector_create(&swapchain_details->formats, sizeof(VkSurfaceFormatKHR));
	if (format_count != 0)
	{
		hyper_vector_resize(&swapchain_details->formats, format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			physical_device, surface, &format_count, swapchain_details->formats.data);
	}

	uint32_t present_mode_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		physical_device, surface, &present_mode_count, NULL);

	hyper_vector_create(
		&swapchain_details->present_modes, sizeof(VkPresentModeKHR));
	if (present_mode_count != 0)
	{
		hyper_vector_resize(&swapchain_details->present_modes, format_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			physical_device,
			surface,
			&present_mode_count,
			swapchain_details->present_modes.data);
	}
}

void hyper_vulkan_swapchain_destroy_details(
	struct hyper_swapchain_details *swapchain_details)
{
	hyper_assert$(swapchain_details != NULL);

	hyper_vector_destroy(&swapchain_details->formats);
	hyper_vector_destroy(&swapchain_details->present_modes);
}
