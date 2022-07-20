/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/vulkan/vulkan_device.h"

#include "hyper_common/assertion.h"
#include "hyper_common/logger.h"
#include "hyper_common/vector.h"

static bool hyper_is_physical_device_suitable(VkPhysicalDevice physical_device)
{
	hyper_assert$(physical_device != VK_NULL_HANDLE);

	struct hyper_queue_families queue_families =
		hyper_find_queue_families(physical_device);

	return queue_families.graphics_family_valid;
}

enum hyper_result hyper_vulkan_device_create(
	struct hyper_vulkan_context *vulkan_context)
{
	hyper_assert$(vulkan_context != NULL);

	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(vulkan_context->instance, &device_count, NULL);

	if (device_count == 0)
	{
		hyper_logger_error$("Failed to find physical device with Vulkan support\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	struct hyper_vector devices = { 0 };
	hyper_vector_create(&devices, sizeof(VkPhysicalDevice));
	hyper_vector_resize(&devices, device_count);
	vkEnumeratePhysicalDevices(
		vulkan_context->instance, &device_count, devices.data);

	for (size_t i = 0; i < devices.size; ++i)
	{
		const VkPhysicalDevice *physical_device = hyper_vector_get(&devices, i);
		if (!hyper_is_physical_device_suitable(*physical_device))
		{
			continue;
		}

		vulkan_context->physical_device = *physical_device;
		break;
	}

	if (vulkan_context->physical_device == VK_NULL_HANDLE)
	{
		hyper_vector_destroy(&devices);

		hyper_logger_error$("Failed to find suitable physical device\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	hyper_vector_destroy(&devices);

	struct hyper_queue_families queue_families =
		hyper_find_queue_families(vulkan_context->physical_device);

	const float queue_priority = 1.0f;
	const VkDeviceQueueCreateInfo queue_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = queue_families.graphics_family,
		.queueCount = 1,
		.pQueuePriorities = &queue_priority,
	};

	const VkPhysicalDeviceFeatures physical_device_features = { 0 };

	const VkDeviceCreateInfo device_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queue_create_info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = 0,
		.ppEnabledExtensionNames = NULL,
		.pEnabledFeatures = &physical_device_features,
	};

	if (
		vkCreateDevice(
			vulkan_context->physical_device,
			&device_create_info,
			NULL,
			&vulkan_context->device) != VK_SUCCESS)
	{
		hyper_logger_error$("Failed to create logical device\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	vkGetDeviceQueue(
		vulkan_context->device,
		queue_families.graphics_family,
		0,
		&vulkan_context->graphics_queue);

	return HYPER_RESULT_SUCCESS;
}

void hyper_vulkan_device_destroy(struct hyper_vulkan_context *vulkan_context)
{
	hyper_assert$(vulkan_context != NULL);

	vkDestroyDevice(vulkan_context->device, NULL);
}

struct hyper_queue_families hyper_find_queue_families(
	VkPhysicalDevice physical_device)
{
	hyper_assert$(physical_device != VK_NULL_HANDLE);

	struct hyper_queue_families queue_families = { 0 };

	uint32_t properties_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		physical_device, &properties_count, NULL);

	struct hyper_vector properties = { 0 };
	hyper_vector_create(&properties, sizeof(VkQueueFamilyProperties));
	hyper_vector_resize(&properties, properties_count);
	vkGetPhysicalDeviceQueueFamilyProperties(
		physical_device, &properties_count, properties.data);

	size_t index = 0;
	for (size_t i = 0; i < properties.size; ++i)
	{
		const VkQueueFamilyProperties *queue_family =
			hyper_vector_get(&properties, i);
		if (queue_family->queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queue_families.graphics_family = index;
			queue_families.graphics_family_valid = true;
		}

		if (queue_families.graphics_family_valid)
		{
			break;
		}

		++index;
	}

	hyper_vector_destroy(&properties);

	return queue_families;
}
