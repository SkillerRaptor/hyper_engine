/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/result.h"
#include "hyper_common/vector.h"
#include "hyper_platform/window.h"

#include <stdbool.h>
#include <volk.h>

struct hyper_vulkan_context
{
	bool validation_layers_enabled;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debug_messenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physical_device;
	VkDevice device;
	VkQueue graphics_queue;
	VkQueue present_queue;
	VkSwapchainKHR swapchain;
	struct hyper_vector swapchain_images;
	VkFormat swapchain_format;
	VkExtent2D swapchain_extent;
};

enum hyper_result hyper_vulkan_context_create(
	struct hyper_vulkan_context *vulkan_context,
	struct hyper_window *window);
void hyper_vulkan_context_destroy(struct hyper_vulkan_context *vulkan_context);
