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
	uint32_t current_frame;

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
	struct hyper_vector swapchain_images_views;
	VkPipelineLayout pipeline_layout;
	VkPipeline pipeline;
	VkCommandPool command_pool;
	struct hyper_vector command_buffers;
	struct hyper_vector image_available_semaphores;
	struct hyper_vector render_finished_semaphores;
	struct hyper_vector in_flight_fences;
};

enum hyper_result hyper_vulkan_context_create(
	struct hyper_vulkan_context *vulkan_context,
	struct hyper_window *window);
void hyper_vulkan_context_destroy(struct hyper_vulkan_context *vulkan_context);

void hyper_vulkan_context_render(struct hyper_vulkan_context *vulkan_context);
