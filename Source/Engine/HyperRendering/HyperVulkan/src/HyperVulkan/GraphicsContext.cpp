/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/GraphicsContext.hpp"

#include <GLFW/glfw3.h>

namespace HyperRendering::HyperVulkan
{
	GraphicsContext::GraphicsContext(HyperGame::EventManager& t_event_manager, HyperPlatform::Window& t_window)
		: IGraphicsContext(t_event_manager, t_window)
		, m_device(m_instance, m_surface)
		, m_swap_chain(m_window, m_device, m_surface)
		, m_graphics_pipeline(m_device.device(), m_swap_chain)
	{
	}

	GraphicsContext::~GraphicsContext()
	{
		auto destroy_semaphores = [this]() -> bool
		{
			if (m_image_available_semaphore == VK_NULL_HANDLE)
			{
				return false;
			}

			vkDestroySemaphore(m_device.device(), m_image_available_semaphore, nullptr);
			HyperCore::Logger::debug("Vulkan image available semaphore was destroyed");
			
			if (m_render_finished_semaphore == VK_NULL_HANDLE)
			{
				return false;
			}

			vkDestroySemaphore(m_device.device(), m_render_finished_semaphore, nullptr);
			HyperCore::Logger::debug("Vulkan render finished semaphore was destroyed");

			return true;
		};

		auto destroy_command_pool = [this]() -> bool
		{
			if (m_command_pool == VK_NULL_HANDLE)
			{
				return false;
			}

			vkDestroyCommandPool(m_device.device(), m_command_pool, nullptr);
			HyperCore::Logger::debug("Vulkan command pool was destroyed");

			return true;
		};

		auto destroy_surface = [this]() -> bool
		{
			if (m_surface == VK_NULL_HANDLE)
			{
				return false;
			}

			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
			HyperCore::Logger::debug("Vulkan surface was destroyed");

			return true;
		};

		auto destroy_debug_messenger = [this]() -> bool
		{
			if (m_validation_layers_enabled)
			{
				if (m_debug_messenger == VK_NULL_HANDLE)
				{
					return false;
				}

				vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
				HyperCore::Logger::debug("Vulkan debug messenger was destroyed");
			}

			return true;
		};

		auto destroy_instance = [this]() -> bool
		{
			if (m_instance == VK_NULL_HANDLE)
			{
				return false;
			}

			vkDestroyInstance(m_instance, nullptr);
			HyperCore::Logger::debug("Vulkan instance was destroyed");

			return true;
		};
		
		vkDeviceWaitIdle(m_device.device());
		
		if (!destroy_semaphores())
		{
			return;
		}

		if (!destroy_command_pool())
		{
			return;
		}

		if (!m_graphics_pipeline.destroy())
		{
			return;
		}

		if (!m_swap_chain.destroy())
		{
			return;
		}

		if (!m_device.destroy())
		{
			return;
		}

		if (!destroy_surface())
		{
			return;
		}

		if (!destroy_debug_messenger())
		{
			return;
		}

		if (!destroy_instance())
		{
			return;
		}

		HyperCore::Logger::info("Successfully destroyed Vulkan context");
	}

	auto GraphicsContext::initialize() -> bool
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsContext::initialize(): Failed to initialize volk");
			return false;
		}

		HyperCore::Logger::debug("Volk was initialized");

		if (m_validation_layers_enabled && !validation_layers_supported())
		{
			HyperCore::Logger::warning("Vulkan validation layer were requested, but are not supported!");
			m_validation_layers_enabled = false;
		}

		if (!create_instance())
		{
			HyperCore::Logger::fatal("GraphicsContext::initialize(): Failed to create vulkan instance");
			return false;
		}

		if (m_validation_layers_enabled)
		{
			if (!create_debug_messenger())
			{
				HyperCore::Logger::fatal("GraphicsContext::initialize(): Failed to create vulkan debug messenger");
				return false;
			}
		}

		if (!create_surface())
		{
			HyperCore::Logger::fatal("GraphicsContext::initialize(): Failed to create vulkan surface");
			return false;
		}

		if (!m_device.initialize())
		{
			HyperCore::Logger::fatal("GraphicsContext::initialize(): Failed to create device");
			return false;
		}

		if (!m_swap_chain.initialize())
		{
			HyperCore::Logger::fatal("GraphicsContext::initialize(): Failed to create swap chain");
			return false;
		}

		if (!m_graphics_pipeline.initialize())
		{
			HyperCore::Logger::fatal("GraphicsContext::initialize(): Failed to create graphics pipeline");
			return false;
		}

		if (!create_command_pool())
		{
			HyperCore::Logger::fatal("GraphicsContext::initialize(): Failed to create command pool");
			return false;
		}

		if (!create_command_buffers())
		{
			HyperCore::Logger::fatal("GraphicsContext::initialize(): Failed to create command buffers");
			return false;
		}

		if (!record_command_buffers())
		{
			HyperCore::Logger::fatal("GraphicsContext::initialize(): Failed to record command buffers");
			return false;
		}

		if (!create_semaphores())
		{
			HyperCore::Logger::fatal("GraphicsContext::initialize(): Failed to create command buffers");
			return false;
		}

		HyperCore::Logger::info("Successfully created Vulkan context");

		return true;
	}

	auto GraphicsContext::update() -> void
	{
		uint32_t next_image_index = 0;
		vkAcquireNextImageKHR(m_device.device(), m_swap_chain.swap_chain(), UINT64_MAX, m_image_available_semaphore, VK_NULL_HANDLE, &next_image_index);
		
		VkSemaphore wait_semaphores[] = { m_image_available_semaphore };
		VkSemaphore signal_semaphores[] = { m_render_finished_semaphore };
		VkPipelineStageFlags pipeline_wait_flags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		
		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitDstStageMask = pipeline_wait_flags;
		submit_info.pCommandBuffers = &m_commands_buffers[next_image_index];
		submit_info.commandBufferCount = 1;
		submit_info.pSignalSemaphores = signal_semaphores;
		submit_info.signalSemaphoreCount = 1;
		
		if (vkQueueSubmit(m_device.graphics_queue(), 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("QUEUE SUBMIT FAILED!");
		}
		
		VkPresentInfoKHR present_info{};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.pWaitSemaphores = signal_semaphores;
		present_info.waitSemaphoreCount = 1;
		
		VkSwapchainKHR swap_chains[] = { m_swap_chain.swap_chain() };
		present_info.pSwapchains = swap_chains;
		present_info.swapchainCount = 1;
		present_info.pImageIndices = &next_image_index;
		present_info.pResults = nullptr;
		
		vkQueuePresentKHR(m_device.present_queue(), &present_info);
		vkQueueWaitIdle(m_device.present_queue());
	}

	auto GraphicsContext::create_instance() -> bool
	{
		VkApplicationInfo application_info{};
		application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		application_info.pApplicationName = m_window.title().c_str();
		application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		application_info.pEngineName = "HyperEngine";
		application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		application_info.apiVersion = VK_API_VERSION_1_2;

		auto required_extensions = get_required_extensions();

		VkInstanceCreateInfo instance_create_info{};
		instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_create_info.pApplicationInfo = &application_info;
		instance_create_info.ppEnabledExtensionNames = required_extensions.data();
		instance_create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
		instance_create_info.ppEnabledLayerNames = nullptr;
		instance_create_info.enabledLayerCount = 0;
		instance_create_info.pNext = nullptr;

		VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
		if (m_validation_layers_enabled)
		{
			instance_create_info.ppEnabledLayerNames = s_validation_layers.data();
			instance_create_info.enabledLayerCount = static_cast<uint32_t>(s_validation_layers.size());

			debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debug_messenger_create_info.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debug_messenger_create_info.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debug_messenger_create_info.pfnUserCallback = debug_callback;

			instance_create_info.pNext = &debug_messenger_create_info;
		}

		if (vkCreateInstance(&instance_create_info, nullptr, &m_instance) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsContext::create_instance(): Failed to create Vulkan instance");
			return false;
		}

		volkLoadInstance(m_instance);

		HyperCore::Logger::debug("Vulkan instance was created");

		return true;
	}

	auto GraphicsContext::create_debug_messenger() -> bool
	{
		VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
		debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debug_messenger_create_info.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debug_messenger_create_info.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debug_messenger_create_info.pfnUserCallback = debug_callback;

		if (vkCreateDebugUtilsMessengerEXT(m_instance, &debug_messenger_create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsContext::create_debug_messenger(): Failed to create Vulkan debug messenger");
			return false;
		}

		HyperCore::Logger::debug("Vulkan debug messenger was created");

		return true;
	}

	auto GraphicsContext::create_surface() -> bool
	{
		if (glfwCreateWindowSurface(m_instance, m_window.native_window(), nullptr, &m_surface) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsContext::create_surface(): Failed to create Vulkan surface");
			return false;
		}

		HyperCore::Logger::debug("Vulkan surface was created");

		return true;
	}

	auto GraphicsContext::create_command_pool() -> bool
	{
		auto queue_family_indices = m_device.find_queue_families(m_device.physical_device());

		VkCommandPoolCreateInfo command_pool_create_info{};
		command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_create_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
		command_pool_create_info.flags = 0;

		if (vkCreateCommandPool(m_device.device(), &command_pool_create_info, nullptr, &m_command_pool) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsContext::create_command_pool(): Failed to create Vulkan command pool");
			return false;
		}

		HyperCore::Logger::debug("Vulkan command pool was created");

		return true;
	}

	auto GraphicsContext::create_command_buffers() -> bool
	{
		m_commands_buffers.resize(m_graphics_pipeline.swap_chain_framebuffers().size());

		VkCommandBufferAllocateInfo command_buffer_allocate_info{};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.commandPool = m_command_pool;
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandBufferCount = static_cast<uint32_t>(m_commands_buffers.size());

		if (vkAllocateCommandBuffers(m_device.device(), &command_buffer_allocate_info, m_commands_buffers.data()) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsContext::create_command_buffers(): Failed to create Vulkan command buffers");
			return false;
		}

		HyperCore::Logger::debug("Vulkan command buffers were created");

		return true;
	}

	auto GraphicsContext::record_command_buffers() -> bool
	{
		for (size_t i = 0; i < m_commands_buffers.size(); ++i)
		{
			VkCommandBufferBeginInfo command_buffer_begin_info{};
			command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			command_buffer_begin_info.flags = 0;
			command_buffer_begin_info.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(m_commands_buffers[i], &command_buffer_begin_info) != VK_SUCCESS)
			{
				HyperCore::Logger::fatal("GraphicsContext::record_command_buffers(): Failed to begin recording Vulkan command buffer #{}", i);
				return false;
			}

			VkRenderPassBeginInfo render_pass_begin_info{};
			render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_begin_info.renderPass = m_graphics_pipeline.render_pass();
			render_pass_begin_info.framebuffer = m_graphics_pipeline.swap_chain_framebuffers()[i];
			render_pass_begin_info.renderArea.offset = { 0, 0 };
			render_pass_begin_info.renderArea.extent = m_swap_chain.extent();

			VkClearValue clear_color_value = { { { 0.0F, 0.0F, 0.0F, 1.0F } } };
			render_pass_begin_info.pClearValues = &clear_color_value;
			render_pass_begin_info.clearValueCount = 1;

			vkCmdBeginRenderPass(m_commands_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(m_commands_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline.graphics_pipeline());
			vkCmdDraw(m_commands_buffers[i], 3, 1, 0, 0);
			vkCmdEndRenderPass(m_commands_buffers[i]);

			if (vkEndCommandBuffer(m_commands_buffers[i]) != VK_SUCCESS)
			{
				HyperCore::Logger::fatal("GraphicsContext::record_command_buffers(): Failed to end recording Vulkan command buffer #{}", i);
				return false;
			}

			HyperCore::Logger::debug("Vulkan command buffer #{} was recorded", i);
		}

		return true;
	}

	auto GraphicsContext::create_semaphores() -> bool
	{
		VkSemaphoreCreateInfo semaphore_create_info{};
		semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(m_device.device(), &semaphore_create_info, nullptr, &m_image_available_semaphore) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsContext::create_semaphores(): Failed to create Vulkan image available semaphore");
			return false;
		}

		HyperCore::Logger::debug("Vulkan image available semaphore was created");

		if (vkCreateSemaphore(m_device.device(), &semaphore_create_info, nullptr, &m_render_finished_semaphore) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsContext::create_semaphores(): Failed to create Vulkan render finished semaphore");
			return false;
		}

		HyperCore::Logger::debug("Vulkan render finished semaphore was created");

		return true;
	}

	auto GraphicsContext::validation_layers_supported() const -> bool
	{
		uint32_t available_layer_count = 0;
		vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr);

		std::vector<VkLayerProperties> available_layer_properties(available_layer_count);
		vkEnumerateInstanceLayerProperties(&available_layer_count, available_layer_properties.data());

		for (const char* validation_layer : s_validation_layers)
		{
			bool layer_found = false;

			for (const auto& layer_properties : available_layer_properties)
			{
				if (strcmp(validation_layer, layer_properties.layerName) != 0)
				{
					continue;
				}

				layer_found = true;
				break;
			}

			if (!layer_found)
			{
				return false;
			}
		}

		return true;
	}

	auto GraphicsContext::get_required_extensions() const -> std::vector<const char*>
	{
		uint32_t required_instance_extension_count = 0;
		const char** required_instance_extensions;
		required_instance_extensions = glfwGetRequiredInstanceExtensions(&required_instance_extension_count);

		std::vector<const char*> required_extensions(required_instance_extensions, required_instance_extensions + required_instance_extension_count);

		if (m_validation_layers_enabled)
		{
			required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return required_extensions;
	}

	auto GraphicsContext::debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity_flags,
		VkDebugUtilsMessageTypeFlagsEXT type_flags,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* user_data)
		-> VkBool32
	{
		HYPERENGINE_VARIABLE_NOT_USED(type_flags);
		HYPERENGINE_VARIABLE_NOT_USED(user_data);

		if (severity_flags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			HyperCore::Logger::fatal("GraphicsContext::debug_callback(): {}", callback_data->pMessage);
		}
		else if (severity_flags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			HyperCore::Logger::warning("GraphicsContext::debug_callback(): {}", callback_data->pMessage);
		}

		return VK_FALSE;
	}
} // namespace HyperRendering::HyperVulkan
