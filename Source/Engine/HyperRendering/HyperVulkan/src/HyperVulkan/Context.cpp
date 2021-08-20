/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/Context.hpp"

#include <GLFW/glfw3.h>
#include <volk.h>

namespace HyperRendering::Vulkan
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity_flags,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void*)
	{
		if (severity_flags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			HyperCore::Logger::fatal("Vulkan Validation Layer: {}", callback_data->pMessage);
		}
		else if (severity_flags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			HyperCore::Logger::warning("Vulkan Validation Layer: {}", callback_data->pMessage);
		}

		return VK_FALSE;
	}

	Context::Context(HyperPlatform::Window& window)
		: IContext(window)
	{
	}

	Context::~Context()
	{
#if HYPERENGINE_DEBUG
		vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
#endif

		vkDestroyInstance(m_instance, nullptr);
	}

	auto Context::initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			HyperCore::Logger::error("Failed to initialize volk!");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

#if HYPERENGINE_DEBUG
		if (!are_validation_layers_supported())
		{
			HyperCore::Logger::error("Failed to find requested validation layer!");
			return HyperCore::Errors::ConstructError::Incomplete;
		}
#endif

		auto instance_result = create_instance();
		if (instance_result.is_error())
		{
			return instance_result;
		}

#if HYPERENGINE_DEBUG
		auto debug_messenger_result = create_debug_messenger();
		if (debug_messenger_result.is_error())
		{
			return debug_messenger_result;
		}
#endif

		return {};
	}

	auto Context::update() -> void
	{
	}

	auto Context::create_instance() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
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
		
#if HYPERENGINE_DEBUG
		instance_create_info.ppEnabledLayerNames = s_validation_layers.data();
		instance_create_info.enabledLayerCount = static_cast<uint32_t>(s_validation_layers.size());
		
		std::array<VkValidationFeatureEnableEXT, 1> enabled_validation_features = { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT };
		VkValidationFeaturesEXT validation_features{};
		validation_features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		validation_features.enabledValidationFeatureCount = static_cast<uint32_t>(enabled_validation_features.size());
		validation_features.pEnabledValidationFeatures = enabled_validation_features.data();
		
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
		debug_messenger_create_info.pNext = &validation_features;
		
		instance_create_info.pNext = &debug_messenger_create_info;
#endif

		if (vkCreateInstance(&instance_create_info, nullptr, &m_instance) != VK_SUCCESS)
		{
			HyperCore::Logger::error("Failed to create VkInstance!");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

		volkLoadInstance(m_instance);

		return {};
	}

	auto Context::create_debug_messenger() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
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
			HyperCore::Logger::error("Failed to create VkDebugUtilsMessengerEXT!");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

		return {};
	}

	auto Context::get_required_extensions() const -> std::vector<const char*>
	{
		uint32_t required_instance_extension_count = 0;
		const char** required_instance_extensions = nullptr;
		required_instance_extensions = glfwGetRequiredInstanceExtensions(&required_instance_extension_count);

		std::vector<const char*> required_extensions(required_instance_extensions, required_instance_extensions + required_instance_extension_count);

#if HYPERENGINE_DEBUG
		required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		return required_extensions;
	}

	auto Context::are_validation_layers_supported() const -> bool
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
} // namespace HyperRendering::Vulkan
