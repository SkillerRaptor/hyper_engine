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
	{
	}

	GraphicsContext::~GraphicsContext()
	{
		if (m_validation_layers_enabled)
		{
			if (m_debug_messenger != VK_NULL_HANDLE)
			{
				vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
				HyperCore::Logger::debug("Vulkan debug messenger was destroyed");
			}
		}

		if (m_instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(m_instance, nullptr);
			HyperCore::Logger::debug("Vulkan instance was destroyed");
		}

		HyperCore::Logger::info("Successfully destroyed Vulkan context");
	}

	auto GraphicsContext::initialize() -> HyperCore::InitializeResult
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			HyperCore::Logger::error("Failed to initialize volk");
			return HyperCore::ConstructError::Incomplete;
		}

		HyperCore::Logger::debug("Volk was initialized");

#if HYPERENGINE_DEBUG
		if (!validation_layers_supported())
		{
			HyperCore::Logger::warning("Vulkan validation layer were requested, but are not supported!");
			m_validation_layers_enabled = false;
		}
#endif

		const auto instance_result = create_instance();
		if (instance_result.is_error())
		{
			HyperCore::Logger::error("Failed to create vulkan instance - {}", instance_result.error());
			return instance_result.error();
		}

		if (m_validation_layers_enabled)
		{
			const auto debug_messenger_result = create_debug_messenger();
			if (debug_messenger_result.is_error())
			{
				HyperCore::Logger::error("Failed to create vulkan debug messenger - {}", debug_messenger_result.error());
				return debug_messenger_result.error();
			}
		}

		HyperCore::Logger::info("Successfully created Vulkan context");

		return {};
	}

	auto GraphicsContext::update() -> void
	{
	}

	auto GraphicsContext::create_instance() -> HyperCore::InitializeResult
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
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
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
			HyperCore::Logger::error("Failed to create Vulkan instance");
			return HyperCore::ConstructError::Incomplete;
		}

		volkLoadInstance(m_instance);

		HyperCore::Logger::debug("Vulkan instance was created");

		return {};
	}

	auto GraphicsContext::create_debug_messenger() -> HyperCore::InitializeResult
	{
		VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
		debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debug_messenger_create_info.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debug_messenger_create_info.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debug_messenger_create_info.pfnUserCallback = debug_callback;

		if (vkCreateDebugUtilsMessengerEXT(m_instance, &debug_messenger_create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
		{
			HyperCore::Logger::error("Failed to create Vulkan debug messenger");
			return HyperCore::ConstructError::Incomplete;
		}

		HyperCore::Logger::debug("Vulkan debug messenger was created");

		return {};
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
		void* user_data) -> VkBool32
	{
		HYPERENGINE_VARIABLE_NOT_USED(type_flags);
		HYPERENGINE_VARIABLE_NOT_USED(user_data);
		
		if (severity_flags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			HyperCore::Logger::fatal("{}", callback_data->pMessage);
		}
		else if (severity_flags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			HyperCore::Logger::warning("{}", callback_data->pMessage);
		}
		else if (severity_flags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		{
			HyperCore::Logger::trace("{}", callback_data->pMessage);
		}
		
		return VK_FALSE;
	}
} // namespace HyperRendering::HyperVulkan
