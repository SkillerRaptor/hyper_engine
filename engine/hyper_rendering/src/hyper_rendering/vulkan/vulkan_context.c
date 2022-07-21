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

static size_t s_max_frames_in_flight = 2;

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

static enum hyper_result hyper_record_command_buffer(
	struct hyper_vulkan_context *vulkan_context,
	VkCommandBuffer command_buffer,
	uint32_t image_index)
{
	const VkCommandBufferBeginInfo command_buffer_begin_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pInheritanceInfo = NULL,
	};

	if (
		vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info) !=
		VK_SUCCESS)
	{
		hyper_logger_error$("Failed to begin recording command buffer\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	const VkClearValue clear_color = {
		.color = {
			.float32 = {
				[0] = 0.0f,
				[1] = 0.0f,
				[2] = 0.0f,
				[3] = 1.0f,
			}
		},
	};

	VkFramebuffer *framebuffer =
		hyper_vector_get(&vulkan_context->swapchain_framebuffers, image_index);
	const VkRenderPassBeginInfo render_pass_begin_info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = vulkan_context->render_pass,
		.framebuffer = *framebuffer,
		.renderArea = {
			.offset = {
				.x = 0,
				.y = 0,
			},
			.extent = vulkan_context->swapchain_extent,
		},
		.clearValueCount = 1,
		.pClearValues = &clear_color,
	};

	vkCmdBeginRenderPass(
		command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(
		command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_context->pipeline);

	const VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = vulkan_context->swapchain_extent.width,
		.height = vulkan_context->swapchain_extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	vkCmdSetViewport(command_buffer, 0, 1, &viewport);

	const VkRect2D scissor = {
		.offset = {
			.x = 0,
			.y = 0,
		},
		.extent = vulkan_context->swapchain_extent,
	};
	vkCmdSetScissor(command_buffer, 0, 1, &scissor);

	vkCmdDraw(command_buffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(command_buffer);

	if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
	{
		hyper_logger_error$("Failed to end recording command buffer\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	return HYPER_RESULT_SUCCESS;
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

	// TODO: Abstract following code into own files
	hyper_vector_create(
		&vulkan_context->swapchain_framebuffers, sizeof(VkFramebuffer));
	hyper_vector_resize(
		&vulkan_context->swapchain_framebuffers,
		vulkan_context->swapchain_images_views.size);

	for (size_t i = 0; i < vulkan_context->swapchain_images_views.size; ++i)
	{
		VkImageView *image_view =
			hyper_vector_get(&vulkan_context->swapchain_images_views, i);
		const VkImageView attachments[] = {
			[0] = *image_view,
		};

		const VkFramebufferCreateInfo framebuffer_create_info = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = vulkan_context->render_pass,
			.attachmentCount = 1,
			.pAttachments = attachments,
			.width = vulkan_context->swapchain_extent.width,
			.height = vulkan_context->swapchain_extent.height,
			.layers = 1,
		};

		VkFramebuffer *framebuffer =
			hyper_vector_get(&vulkan_context->swapchain_framebuffers, i);
		if (
			vkCreateFramebuffer(
				vulkan_context->device, &framebuffer_create_info, NULL, framebuffer) !=
			VK_SUCCESS)
		{
			hyper_logger_error$("Failed to create framebuffer #%u\n", i);
			return HYPER_RESULT_INITIALIZATION_FAILED;
		}
	}

	struct hyper_queue_families queue_families = { 0 };
	hyper_vulkan_device_find_queue_families(
		&queue_families, vulkan_context->physical_device, vulkan_context->surface);

	const VkCommandPoolCreateInfo command_pool_create_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queue_families.graphics_family,
	};

	if (
		vkCreateCommandPool(
			vulkan_context->device,
			&command_pool_create_info,
			NULL,
			&vulkan_context->command_pool) != VK_SUCCESS)
	{
		hyper_logger_error$("Failed to create command pool\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	hyper_vector_create(
		&vulkan_context->command_buffers, sizeof(VkCommandBuffer));
	hyper_vector_resize(&vulkan_context->command_buffers, s_max_frames_in_flight);

	const VkCommandBufferAllocateInfo command_buffer_allocate_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = vulkan_context->command_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = vulkan_context->command_buffers.size,
	};

	if (
		vkAllocateCommandBuffers(
			vulkan_context->device,
			&command_buffer_allocate_info,
			vulkan_context->command_buffers.data) != VK_SUCCESS)
	{
		hyper_logger_error$("Failed to allocate command buffers\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	hyper_vector_create(
		&vulkan_context->image_available_semaphores, sizeof(VkSemaphore));
	hyper_vector_resize(
		&vulkan_context->image_available_semaphores, s_max_frames_in_flight);

	const VkSemaphoreCreateInfo semaphore_create_info = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	};

	for (size_t i = 0; i < s_max_frames_in_flight; ++i)
	{
		VkSemaphore *semaphore =
			hyper_vector_get(&vulkan_context->image_available_semaphores, i);
		if (
			vkCreateSemaphore(
				vulkan_context->device, &semaphore_create_info, NULL, semaphore) !=
			VK_SUCCESS)
		{
			hyper_logger_error$(
				"Failed to create image available semaphore #%u\n", i);
			return HYPER_RESULT_INITIALIZATION_FAILED;
		}
	}

	hyper_vector_create(
		&vulkan_context->render_finished_semaphores, sizeof(VkSemaphore));
	hyper_vector_resize(
		&vulkan_context->render_finished_semaphores, s_max_frames_in_flight);

	for (size_t i = 0; i < s_max_frames_in_flight; ++i)
	{
		VkSemaphore *semaphore =
			hyper_vector_get(&vulkan_context->render_finished_semaphores, i);
		if (
			vkCreateSemaphore(
				vulkan_context->device, &semaphore_create_info, NULL, semaphore) !=
			VK_SUCCESS)
		{
			hyper_logger_error$(
				"Failed to create render finished semaphore #%u\n", i);
			return HYPER_RESULT_INITIALIZATION_FAILED;
		}
	}

	hyper_vector_create(&vulkan_context->in_flight_fences, sizeof(VkFence));
	hyper_vector_resize(
		&vulkan_context->in_flight_fences, s_max_frames_in_flight);

	const VkFenceCreateInfo fence_create_info = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT,
	};

	for (size_t i = 0; i < s_max_frames_in_flight; ++i)
	{
		VkFence *fence = hyper_vector_get(&vulkan_context->in_flight_fences, i);
		if (
			vkCreateFence(vulkan_context->device, &fence_create_info, NULL, fence) !=
			VK_SUCCESS)
		{
			hyper_logger_error$("Failed to create in flight fence #%u\n", i);
			return HYPER_RESULT_INITIALIZATION_FAILED;
		}
	}

	return HYPER_RESULT_SUCCESS;
}

void hyper_vulkan_context_destroy(struct hyper_vulkan_context *vulkan_context)
{
	hyper_assert$(vulkan_context != NULL);

	vkDeviceWaitIdle(vulkan_context->device);

	for (size_t i = 0; i < s_max_frames_in_flight; ++i)
	{
		VkFence *in_flight_fence =
			hyper_vector_get(&vulkan_context->in_flight_fences, i);
		vkDestroyFence(vulkan_context->device, *in_flight_fence, NULL);

		VkSemaphore *render_finished_semaphore =
			hyper_vector_get(&vulkan_context->render_finished_semaphores, i);
		vkDestroySemaphore(
			vulkan_context->device, *render_finished_semaphore, NULL);

		VkSemaphore *image_available_semaphore =
			hyper_vector_get(&vulkan_context->image_available_semaphores, i);
		vkDestroySemaphore(
			vulkan_context->device, *image_available_semaphore, NULL);
	}

	hyper_vector_destroy(&vulkan_context->in_flight_fences);
	hyper_vector_destroy(&vulkan_context->render_finished_semaphores);
	hyper_vector_destroy(&vulkan_context->image_available_semaphores);
	hyper_vector_destroy(&vulkan_context->command_buffers);

	vkDestroyCommandPool(
		vulkan_context->device, vulkan_context->command_pool, NULL);

	for (size_t i = 0; i < vulkan_context->swapchain_framebuffers.size; ++i)
	{
		VkFramebuffer *framebuffer =
			hyper_vector_get(&vulkan_context->swapchain_framebuffers, i);
		vkDestroyFramebuffer(vulkan_context->device, *framebuffer, NULL);
	}

	hyper_vector_destroy(&vulkan_context->swapchain_framebuffers);

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

void hyper_vulkan_context_render(struct hyper_vulkan_context *vulkan_context)
{
	VkFence *in_flight_fence = hyper_vector_get(
		&vulkan_context->in_flight_fences, vulkan_context->current_frame);
	vkWaitForFences(
		vulkan_context->device, 1, &*in_flight_fence, VK_TRUE, UINT64_MAX);
	vkResetFences(vulkan_context->device, 1, &*in_flight_fence);

	VkSemaphore *image_available_semaphore = hyper_vector_get(
		&vulkan_context->image_available_semaphores, vulkan_context->current_frame);
	uint32_t image_index = 0;
	vkAcquireNextImageKHR(
		vulkan_context->device,
		vulkan_context->swapchain,
		UINT64_MAX,
		*image_available_semaphore,
		VK_NULL_HANDLE,
		&image_index);

	VkCommandBuffer *command_buffer = hyper_vector_get(
		&vulkan_context->command_buffers, vulkan_context->current_frame);
	vkResetCommandBuffer(*command_buffer, 0);
	if (
		hyper_record_command_buffer(vulkan_context, *command_buffer, image_index) !=
		HYPER_RESULT_SUCCESS)
	{
		hyper_logger_error$("Failed to record command buffer\n");
		return;
	}

	const VkSemaphore wait_semaphores[] = {
		[0] = *image_available_semaphore,
	};

	const VkPipelineStageFlags wait_stages[] = {
		[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	};

	VkSemaphore *render_finished_semaphore = hyper_vector_get(
		&vulkan_context->render_finished_semaphores, vulkan_context->current_frame);
	const VkSemaphore signal_semaphores[] = {
		[0] = *render_finished_semaphore,
	};

	const VkSubmitInfo submit_info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = wait_semaphores,
		.pWaitDstStageMask = wait_stages,
		.commandBufferCount = 1,
		.pCommandBuffers = &*command_buffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signal_semaphores,
	};

	if (
		vkQueueSubmit(
			vulkan_context->graphics_queue, 1, &submit_info, *in_flight_fence) !=
		VK_SUCCESS)
	{
		hyper_logger_error$("Failed to submit command buffer queue\n");
		return;
	}

	const VkSwapchainKHR swapchains[] = {
		[0] = vulkan_context->swapchain,
	};

	const VkPresentInfoKHR present_info = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signal_semaphores,
		.swapchainCount = 1,
		.pSwapchains = swapchains,
		.pImageIndices = &image_index,
		.pResults = NULL,
	};

	vkQueuePresentKHR(vulkan_context->present_queue, &present_info);

	vulkan_context->current_frame =
		(vulkan_context->current_frame + 1) % s_max_frames_in_flight;
}
