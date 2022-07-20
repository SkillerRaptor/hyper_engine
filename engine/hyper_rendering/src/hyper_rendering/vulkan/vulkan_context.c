/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/vulkan/vulkan_context.h"

#include "hyper_common/assertion.h"
#include "hyper_common/debug.h"
#include "hyper_common/logger.h"
#include "hyper_common/memory.h"
#include "hyper_common/prerequisites.h"
#include "hyper_common/vector.h"
#include "hyper_rendering/vulkan/vulkan_device.h"
#include "hyper_rendering/vulkan/vulkan_pipeline.h"
#include "hyper_rendering/vulkan/vulkan_swapchain.h"

#include <string.h>

static const char *s_validation_layers[] = {
	"VK_LAYER_KHRONOS_validation",
};

static bool hyper_validation_layers_supported()
{
	uint32_t layer_count = 0;
	vkEnumerateInstanceLayerProperties(&layer_count, NULL);

	struct hyper_vector layers = { 0 };
	hyper_vector_create(&layers, sizeof(VkLayerProperties));
	hyper_vector_resize(&layers, layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, layers.data);

	for (size_t i = 0; i < hyper_array_size$(s_validation_layers); ++i)
	{
		const char *validation_layer = s_validation_layers[i];

		bool layer_found = false;
		for (size_t j = 0; j < layers.size; ++j)
		{
			const VkLayerProperties *layer_properties = hyper_vector_get(&layers, j);

			if (
				strncmp(
					validation_layer,
					layer_properties->layerName,
					strlen(validation_layer)) == 0)
			{
				layer_found = true;
				break;
			}
		}

		if (!layer_found)
		{
			hyper_vector_destroy(&layers);
			return false;
		}
	}

	hyper_vector_destroy(&layers);
	return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL hyper_debug_messenger_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
	void *user_data)
{
	hyper_logger_error$("Validation layer: %s\n", callback_data->pMessage);
	return VK_FALSE;
}

enum hyper_result hyper_vulkan_context_create(
	struct hyper_vulkan_context *vulkan_context,
	struct hyper_window *window)
{
	hyper_assert$(vulkan_context != NULL);
	hyper_assert$(window != NULL);

	if (volkInitialize() != VK_SUCCESS)
	{
		hyper_logger_error$("Failed to initialize volk\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	const VkApplicationInfo application_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "HyperEngine",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "HyperEngine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3,
	};

#if HYPER_DEBUG
	if (hyper_validation_layers_supported())
	{
		vulkan_context->validation_layers_enabled = true;
	}
#endif

	const VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
											 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
											 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = hyper_debug_messenger_callback,
	};

	const void *instance_next =
		(vulkan_context->validation_layers_enabled ? &debug_messenger_create_info
																							 : NULL);

	const uint32_t enabled_layer_count =
		(vulkan_context->validation_layers_enabled
			 ? hyper_array_size$(s_validation_layers)
			 : 0);
	const char *const *enabled_layers =
		(vulkan_context->validation_layers_enabled ? &s_validation_layers[0]
																							 : NULL);

	uint32_t required_extension_count = 0;
	const char **required_extensions = NULL;
	hyper_window_get_required_extensions(
		window, &required_extensions, &required_extension_count);

	uint32_t enabled_extension_count = required_extension_count;
	const char *const *enabled_extensions = required_extensions;

	struct hyper_vector extensions = { 0 };
	if (vulkan_context->validation_layers_enabled)
	{
		hyper_vector_create(&extensions, sizeof(char *));
		hyper_vector_resize(&extensions, required_extension_count + 1);

		for (size_t i = 0; i < required_extension_count; ++i)
		{
			char **string_ptr = (char **) hyper_vector_get(&extensions, i);
			*string_ptr =
				hyper_allocate((strlen(required_extensions[i]) + 1) * sizeof(char));
			strcpy(*string_ptr, required_extensions[i]);
		}

		char **string_ptr =
			(char **) hyper_vector_get(&extensions, required_extension_count);
		*string_ptr = hyper_allocate(
			(strlen(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) + 1) * sizeof(char));
		strcpy(*string_ptr, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		enabled_extension_count = extensions.size;
		enabled_extensions = extensions.data;
	}

	const VkInstanceCreateInfo instance_create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = instance_next,
		.pApplicationInfo = &application_info,
		.enabledLayerCount = enabled_layer_count,
		.ppEnabledLayerNames = enabled_layers,
		.enabledExtensionCount = enabled_extension_count,
		.ppEnabledExtensionNames = enabled_extensions,
	};

	if (
		vkCreateInstance(&instance_create_info, NULL, &vulkan_context->instance) !=
		VK_SUCCESS)
	{
		for (size_t i = 0; i < extensions.size; ++i)
		{
			char *string = *(char **) hyper_vector_get(&extensions, i);
			hyper_deallocate(string);
		}

		hyper_vector_destroy(&extensions);

		hyper_logger_error$("Failed to create instance\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	volkLoadInstance(vulkan_context->instance);

	if (vulkan_context->validation_layers_enabled)
	{
		for (size_t i = 0; i < extensions.size; ++i)
		{
			char *string = *(char **) hyper_vector_get(&extensions, i);
			hyper_deallocate(string);
		}

		hyper_vector_destroy(&extensions);

		if (
			vkCreateDebugUtilsMessengerEXT(
				vulkan_context->instance,
				&debug_messenger_create_info,
				NULL,
				&vulkan_context->debug_messenger) != VK_SUCCESS)
		{
			hyper_logger_error$("Failed to create debug messenger\n");
			return HYPER_RESULT_INITIALIZATION_FAILED;
		}
	}

	if (!hyper_window_create_window_surface(
				window, vulkan_context->instance, &vulkan_context->surface))
	{
		hyper_logger_error$("Failed to create surface\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (hyper_vulkan_device_create(vulkan_context) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$("Failed to create device\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (
		hyper_vulkan_swapchain_create(vulkan_context, window) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$("Failed to create swapchain\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	if (hyper_vulkan_pipeline_create(vulkan_context) != HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$("Failed to create pipeline\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	return HYPER_RESULT_SUCCESS;
}

void hyper_vulkan_context_destroy(struct hyper_vulkan_context *vulkan_context)
{
	hyper_assert$(vulkan_context != NULL);

	hyper_vulkan_pipeline_destroy(vulkan_context);
	hyper_vulkan_swapchain_destroy(vulkan_context);
	hyper_vulkan_device_destroy(vulkan_context);

	vkDestroySurfaceKHR(vulkan_context->instance, vulkan_context->surface, NULL);

	if (vulkan_context->validation_layers_enabled)
	{
		vkDestroyDebugUtilsMessengerEXT(
			vulkan_context->instance, vulkan_context->debug_messenger, NULL);
	}

	vkDestroyInstance(vulkan_context->instance, NULL);
}
