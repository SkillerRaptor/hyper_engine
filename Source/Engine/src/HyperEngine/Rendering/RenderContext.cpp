/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/RenderContext.hpp"

#include "HyperEngine/Rendering/Utils.hpp"

#include <GLFW/glfw3.h>
#include <volk.h>

#include <cstring>

namespace HyperEngine
{
	RenderContext::RenderContext(bool validation_layers_enabled, Error &error)
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			error = Error("failed to initialize volk");
			return;
		}

		if (validation_layers_enabled && validation_layers_supported())
		{
			m_validation_layers_enabled = true;
		}

		auto instance = create_instance();
		if (instance.is_error())
		{
			error = instance.error();
			return;
		}

		if (m_validation_layers_enabled)
		{
			auto debug_messenger = create_debug_messenger();
			if (debug_messenger.is_error())
			{
				error = debug_messenger.error();
				return;
			}
		}
	}

	RenderContext::~RenderContext()
	{
		if (m_debug_messenger != nullptr)
		{
			vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
		}

		if (m_instance != nullptr)
		{
			vkDestroyInstance(m_instance, nullptr);
		}
	}

	RenderContext::RenderContext(RenderContext &&other) noexcept
	{
		m_instance = other.m_instance;
		m_debug_messenger = other.m_debug_messenger;

		other.m_instance = nullptr;
		other.m_debug_messenger = nullptr;
	}

	RenderContext &RenderContext::operator=(RenderContext &&other) noexcept
	{
		m_instance = other.m_instance;
		m_debug_messenger = other.m_debug_messenger;

		other.m_instance = nullptr;
		other.m_debug_messenger = nullptr;

		return *this;
	}

	Expected<void> RenderContext::create_instance()
	{
		const VkApplicationInfo application_info = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "HyperEditor",
			.applicationVersion = Vulkan::make_version(1, 0, 0),
			.pEngineName = "HyperEngine",
			.engineVersion = Vulkan::make_version(1, 0, 0),
			.apiVersion = Vulkan::make_api_version(0, 1, 2, 0),
		};

		const VkDebugUtilsMessageSeverityFlagsEXT message_severity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		const VkDebugUtilsMessageTypeFlagsEXT message_type =
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		const VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = nullptr,
			.flags = 0,
			.messageSeverity = message_severity,
			.messageType = message_type,
			.pfnUserCallback = Vulkan::debug_callback,
			.pUserData = nullptr,
		};

		const std::vector<const char *> extensions = request_required_extensions();

		const uint32_t enabled_layer_count =
			m_validation_layers_enabled
				? static_cast<uint32_t>(s_validation_layers.size())
				: 0;
		const char *const *enabled_layers =
			m_validation_layers_enabled ? s_validation_layers.data() : nullptr;
		const VkInstanceCreateInfo instance_create_info = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = m_validation_layers_enabled ? &debug_messenger_info : nullptr,
			.flags = 0,
			.pApplicationInfo = &application_info,
			.enabledLayerCount = enabled_layer_count,
			.ppEnabledLayerNames = enabled_layers,
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
		};

		const VkResult instance_result =
			vkCreateInstance(&instance_create_info, nullptr, &m_instance);
		if (instance_result != VK_SUCCESS)
		{
			return Error("failed to create instance");
		}

		volkLoadInstance(m_instance);

		return {};
	}

	Expected<void> RenderContext::create_debug_messenger()
	{
		const VkDebugUtilsMessageSeverityFlagsEXT message_severity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		const VkDebugUtilsMessageTypeFlagsEXT message_type =
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		const VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = nullptr,
			.flags = 0,
			.messageSeverity = message_severity,
			.messageType = message_type,
			.pfnUserCallback = Vulkan::debug_callback,
			.pUserData = nullptr,
		};

		const VkResult debug_messenger_result = vkCreateDebugUtilsMessengerEXT(
			m_instance, &debug_messenger_info, nullptr, &m_debug_messenger);
		if (debug_messenger_result != VK_SUCCESS)
		{
			return Error("failed to create debug messenger");
		}

		return {};
	}

	bool RenderContext::validation_layers_supported() const
	{
		uint32_t instance_layer_count = 0;
		vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr);

		std::vector<VkLayerProperties> instance_layers(instance_layer_count);
		vkEnumerateInstanceLayerProperties(
			&instance_layer_count, instance_layers.data());

		for (const char *validation_layer : s_validation_layers)
		{
			bool is_layer_found = false;

			for (const VkLayerProperties &layer_properties : instance_layers)
			{
				if (strcmp(validation_layer, layer_properties.layerName) != 0)
				{
					continue;
				}

				is_layer_found = true;
				break;
			}

			if (!is_layer_found)
			{
				return false;
			}
		}

		return true;
	}

	std::vector<const char *> RenderContext::request_required_extensions() const
	{
		uint32_t instance_extension_count = 0;
		const char **instance_extensions =
			glfwGetRequiredInstanceExtensions(&instance_extension_count);

		std::vector<const char *> extensions(
			instance_extensions, instance_extensions + instance_extension_count);

		if (m_validation_layers_enabled)
		{
			extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	Expected<RenderContext> RenderContext::create(bool validation_layers_enabled)
	{
		Error error = Error::success();
		RenderContext render_context(validation_layers_enabled, error);
		if (error.is_error())
		{
			return error;
		}

		return render_context;
	}
} // namespace HyperEngine
