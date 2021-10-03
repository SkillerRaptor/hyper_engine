/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/Context.hpp"

#include "HyperEngine/Core/Logger.hpp"
#include "HyperEngine/Rendering/Vulkan/Debug.hpp"

#include <GLFW/glfw3.h>

namespace HyperEngine::Vulkan
{
	CContext::~CContext()
	{
		for (CFrameBuffer& frame_buffer : m_frame_buffers)
		{
			frame_buffer.destroy();
		}

		m_render_pass.destroy();
		m_command_buffer.destroy();
		m_swapchain.destroy();
		m_device.destroy();
		m_surface.destroy();

		if (m_validation_layers_enabled)
		{
			if (m_debug_messenger != VK_NULL_HANDLE)
			{
				vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
			}
		}

		if (m_instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(m_instance, nullptr);
		}
	}

	auto CContext::create(const CContext::SDescription& description) -> bool
	{
		if (description.window == nullptr)
		{
			CLogger::fatal("CContext::create(): The description window is null");
			return false;
		}

		m_window = description.window;

		if (volkInitialize() != VK_SUCCESS)
		{
			CLogger::fatal("CContext::create(): Failed to initialize volk");
			return false;
		}

		if (description.debug_mode && check_validation_layers_support())
		{
			m_validation_layers_enabled = true;
		}

		if (!create_instance())
		{
			CLogger::fatal("CContext::create(): Failed to create vulkan instance");
			return false;
		}

		if (m_validation_layers_enabled && !create_debug_messenger())
		{
			CLogger::fatal("CContext::create(): Failed to create vulkan debug messenger");
			return false;
		}

		CSurface::SDescription surface_description{};
		surface_description.instance = m_instance;
		surface_description.window = m_window;

		if (!m_surface.create(surface_description))
		{
			CLogger::fatal("CContext::create(): Failed to create surface");
			return false;
		}

		CDevice::SDescription device_description{};
		device_description.instance = m_instance;
		device_description.surface = m_surface.surface();

		if (!m_device.create(device_description))
		{
			CLogger::fatal("CContext::create(): Failed to create device");
			return false;
		}

		CSwapchain::SDescription swapchain_description{};
		swapchain_description.device = m_device.device();
		swapchain_description.surface = m_surface.surface();
		swapchain_description.window = m_window;
		swapchain_description.swapchain_support_details = m_device.query_swapchain_support(m_device.physical_device());
		swapchain_description.queue_families = m_device.find_queue_families(m_device.physical_device());

		if (!m_swapchain.create(swapchain_description))
		{
			CLogger::fatal("CContext::create(): Failed to create swapchain");
			return false;
		}

		CCommandBuffer::SDescription command_buffer_description{};
		command_buffer_description.device = m_device.device();
		command_buffer_description.queue_families = m_device.find_queue_families(m_device.physical_device());

		if (!m_command_buffer.create(command_buffer_description))
		{
			CLogger::fatal("CContext::create(): Failed to create command buffer");
			return false;
		}

		CRenderPass::SDescription render_pass_description{};
		render_pass_description.device = m_device.device();
		render_pass_description.swapchain_image_format = m_swapchain.image_format();

		if (!m_render_pass.create(render_pass_description))
		{
			CLogger::fatal("CContext::create(): Failed to create render pass");
			return false;
		}

		const std::vector<VkImageView> swapchain_image_views = m_swapchain.image_views();
		for (size_t i = 0; i < swapchain_image_views.size(); ++i)
		{
			CFrameBuffer::SDescription frame_buffer_description{};
			frame_buffer_description.device = m_device.device();
			frame_buffer_description.render_pass = m_render_pass.render_pass();
			frame_buffer_description.swapchain_image_view = swapchain_image_views[i];
			frame_buffer_description.swapchain_extent = m_swapchain.extent();

			CFrameBuffer& frame_buffer = m_frame_buffers.emplace_back();
			if (!frame_buffer.create(frame_buffer_description))
			{
				CLogger::fatal("CContext::create(): Failed to create frame buffer #{}", i);
				return false;
			}
		}

		return true;
	}

	auto CContext::create_instance() -> bool
	{
		VkApplicationInfo application_info{};
		application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		application_info.pNext = nullptr;
		application_info.pApplicationName = "HyperEngine";
		application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		application_info.pEngineName = "HyperEngine";
		application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		application_info.apiVersion = VK_API_VERSION_1_2;

		VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
		debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debug_messenger_create_info.pNext = nullptr;
		debug_messenger_create_info.flags = 0;
		debug_messenger_create_info.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debug_messenger_create_info.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debug_messenger_create_info.pfnUserCallback = debug_callback;
		debug_messenger_create_info.pUserData = nullptr;

		const std::vector<const char*> required_extensions = request_required_extensions();

		VkInstanceCreateInfo instance_create_info{};
		instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_create_info.pNext = m_validation_layers_enabled ? &debug_messenger_create_info : nullptr;
		instance_create_info.flags = 0;
		instance_create_info.pApplicationInfo = &application_info;
		instance_create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers_enabled ? s_validation_layers.size() : 0);
		instance_create_info.ppEnabledLayerNames = m_validation_layers_enabled ? s_validation_layers.data() : nullptr;
		instance_create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
		instance_create_info.ppEnabledExtensionNames = required_extensions.data();

		if (vkCreateInstance(&instance_create_info, nullptr, &m_instance) != VK_SUCCESS)
		{
			CLogger::fatal("CContext::create_instance(): Failed to create vulkan instance");
			return false;
		}

		volkLoadInstance(m_instance);

		return true;
	}

	auto CContext::create_debug_messenger() -> bool
	{
		VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
		debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debug_messenger_create_info.pNext = nullptr;
		debug_messenger_create_info.flags = 0;
		debug_messenger_create_info.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debug_messenger_create_info.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debug_messenger_create_info.pfnUserCallback = debug_callback;
		debug_messenger_create_info.pUserData = nullptr;

		if (vkCreateDebugUtilsMessengerEXT(m_instance, &debug_messenger_create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
		{
			CLogger::fatal("CContext::create_debug_messenger(): Failed to create vulkan debug messenger");
			return false;
		}

		return true;
	}

	auto CContext::check_validation_layers_support() const -> bool
	{
		uint32_t instance_layer_properties_count = 0;
		vkEnumerateInstanceLayerProperties(&instance_layer_properties_count, nullptr);

		if (instance_layer_properties_count == 0)
		{
			return false;
		}

		std::vector<VkLayerProperties> instance_layer_properties(instance_layer_properties_count);
		vkEnumerateInstanceLayerProperties(&instance_layer_properties_count, instance_layer_properties.data());

		for (const char* validation_layer : s_validation_layers)
		{
			bool layer_found = false;

			for (const VkLayerProperties& layer_properties : instance_layer_properties)
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

	auto CContext::request_required_extensions() -> std::vector<const char*>
	{
		uint32_t required_instance_extension_count = 0;
		const char** required_instance_extensions = glfwGetRequiredInstanceExtensions(&required_instance_extension_count);

		std::vector<const char*> required_extensions(required_instance_extensions, required_instance_extensions + required_instance_extension_count);

		if (m_validation_layers_enabled)
		{
			required_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return required_extensions;
	}
} // namespace HyperEngine::Vulkan
