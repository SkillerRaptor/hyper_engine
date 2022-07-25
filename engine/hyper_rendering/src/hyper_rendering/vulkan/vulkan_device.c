/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/vulkan/vulkan_device.h"

#include "hyper_common/array.h"
#include "hyper_common/assertion.h"
#include "hyper_common/logger.h"
#include "hyper_common/vector.h"
#include "hyper_rendering/vulkan/vulkan_swapchain.h"

#include <string.h>

static const char *s_physical_device_extensions[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

static bool hyper_physical_device_extensions_supported(
	VkPhysicalDevice physical_device)
{
	hyper_assert$(physical_device != VK_NULL_HANDLE);

	uint32_t extension_count = 0;
	vkEnumerateDeviceExtensionProperties(
		physical_device, NULL, &extension_count, NULL);

	struct hyper_vector extensions = { 0 };
	hyper_vector_create(&extensions, sizeof(VkExtensionProperties));
	hyper_vector_resize(&extensions, extension_count);
	vkEnumerateDeviceExtensionProperties(
		physical_device, NULL, &extension_count, extensions.data);

	for (size_t i = 0; i < hyper_array_size$(s_physical_device_extensions); ++i)
	{
		const char *extension_name = s_physical_device_extensions[i];

		bool extension_found = false;
		hyper_vector_foreach$(
			extensions, const VkExtensionProperties, extension_properties)
		{
			if (
				strncmp(
					extension_name,
					extension_properties->extensionName,
					strlen(extension_name)) == 0)
			{
				extension_found = true;
				break;
			}
		}

		if (!extension_found)
		{
			hyper_vector_destroy(&extensions);
			return false;
		}
	}

	hyper_vector_destroy(&extensions);
	return true;
}

static bool hyper_is_physical_device_suitable(
	VkPhysicalDevice physical_device,
	VkSurfaceKHR surface)
{
	hyper_assert$(physical_device != VK_NULL_HANDLE);

	struct hyper_queue_families queue_families = { 0 };
	hyper_vulkan_device_find_queue_families(
		&queue_families, physical_device, surface);

	const bool extensions_supported =
		hyper_physical_device_extensions_supported(physical_device);

	bool swapchain_valid = false;
	if (extensions_supported)
	{
		struct hyper_swapchain_details swapchain_details = { 0 };
		hyper_vulkan_swapchain_query_details(
			&swapchain_details, physical_device, surface);

		swapchain_valid =
			(swapchain_details.formats.size != 0 &&
			 swapchain_details.present_modes.size);

		hyper_vulkan_swapchain_destroy_details(&swapchain_details);
	}

	return queue_families.graphics_family_valid &&
				 queue_families.present_family_valid && extensions_supported &&
				 swapchain_valid;
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

	hyper_vector_foreach$(devices, const VkPhysicalDevice, physical_device)
	{
		if (!hyper_is_physical_device_suitable(
					*physical_device, vulkan_context->surface))
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

	struct hyper_queue_families queue_families = { 0 };
	hyper_vulkan_device_find_queue_families(
		&queue_families, vulkan_context->physical_device, vulkan_context->surface);

	struct hyper_vector queue_create_infos = { 0 };
	hyper_vector_create(&queue_create_infos, sizeof(VkDeviceQueueCreateInfo));

	const float queue_priority = 1.0f;

	const VkDeviceQueueCreateInfo graphics_queue_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = queue_families.graphics_family,
		.queueCount = 1,
		.pQueuePriorities = &queue_priority,
	};

	hyper_vector_push_back(&queue_create_infos, &graphics_queue_create_info);

	if (queue_families.graphics_family != queue_families.present_family)
	{
		const VkDeviceQueueCreateInfo present_queue_create_info = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = queue_families.present_family,
			.queueCount = 1,
			.pQueuePriorities = &queue_priority,
		};

		hyper_vector_push_back(&queue_create_infos, &present_queue_create_info);
	}

	const VkPhysicalDeviceFeatures physical_device_features = { 0 };

	const VkDeviceCreateInfo device_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = (uint32_t) queue_create_infos.size,
		.pQueueCreateInfos = queue_create_infos.data,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = hyper_array_size$(s_physical_device_extensions),
		.ppEnabledExtensionNames = s_physical_device_extensions,
		.pEnabledFeatures = &physical_device_features,
	};

	if (
		vkCreateDevice(
			vulkan_context->physical_device,
			&device_create_info,
			NULL,
			&vulkan_context->device) != VK_SUCCESS)
	{
		hyper_vector_destroy(&queue_create_infos);

		hyper_logger_error$("Failed to create logical device\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	hyper_vector_destroy(&queue_create_infos);

	vkGetDeviceQueue(
		vulkan_context->device,
		queue_families.graphics_family,
		0,
		&vulkan_context->graphics_queue);
	vkGetDeviceQueue(
		vulkan_context->device,
		queue_families.present_family,
		0,
		&vulkan_context->present_queue);

	return HYPER_RESULT_SUCCESS;
}

void hyper_vulkan_device_destroy(struct hyper_vulkan_context *vulkan_context)
{
	hyper_assert$(vulkan_context != NULL);

	vkDestroyDevice(vulkan_context->device, NULL);
}

void hyper_vulkan_device_find_queue_families(
	struct hyper_queue_families *queue_families,
	VkPhysicalDevice physical_device,
	VkSurfaceKHR surface)
{
	hyper_assert$(physical_device != VK_NULL_HANDLE);

	uint32_t properties_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		physical_device, &properties_count, NULL);

	struct hyper_vector properties = { 0 };
	hyper_vector_create(&properties, sizeof(VkQueueFamilyProperties));
	hyper_vector_resize(&properties, properties_count);
	vkGetPhysicalDeviceQueueFamilyProperties(
		physical_device, &properties_count, properties.data);

	size_t index = 0;
	hyper_vector_foreach$(properties, const VkQueueFamilyProperties, queue_family)
	{
		if (queue_family->queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queue_families->graphics_family = (uint32_t) index;
			queue_families->graphics_family_valid = true;
		}

		VkBool32 present_family_supported = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(
			physical_device, (uint32_t) vector_index, surface, &present_family_supported);

		if (present_family_supported)
		{
			queue_families->present_family = (uint32_t) index;
			queue_families->present_family_valid = true;
		}

		if (
			queue_families->graphics_family_valid &&
			queue_families->present_family_valid)
		{
			break;
		}

		++index;
	}

	hyper_vector_destroy(&properties);
}
