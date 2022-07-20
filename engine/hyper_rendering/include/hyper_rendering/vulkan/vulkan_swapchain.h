/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/result.h"
#include "hyper_common/vector.h"
#include "hyper_platform/window.h"
#include "hyper_rendering/vulkan/vulkan_context.h"

struct hyper_swapchain_details
{
	VkSurfaceCapabilitiesKHR capabilities;
	struct hyper_vector formats;
	struct hyper_vector present_modes;
};

enum hyper_result hyper_vulkan_swapchain_create(
	struct hyper_vulkan_context *vulkan_context,
	struct hyper_window *window);
void hyper_vulkan_swapchain_destroy(
	struct hyper_vulkan_context *vulkan_context);

void hyper_vulkan_swapchain_query_details(
	struct hyper_swapchain_details *swapchain_details,
	VkPhysicalDevice physical_device,
	VkSurfaceKHR surface);
void hyper_vulkan_swapchain_destroy_details(
	struct hyper_swapchain_details *swapchain_details);
