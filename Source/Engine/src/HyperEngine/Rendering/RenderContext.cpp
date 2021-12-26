/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/RenderContext.hpp"

#include "HyperEngine/Logger.hpp"
#include "HyperEngine/Rendering/Utils.hpp"

#include <cstring>
#include <GLFW/glfw3.h>
#include <volk.h>

namespace HyperEngine
{
	RenderContext::RenderContext(RenderContext &&other) noexcept
		: m_instance(std::exchange(other.m_instance, nullptr))
		, m_debug_messenger(std::exchange(other.m_debug_messenger, nullptr))
		, m_surface(std::exchange(other.m_surface, nullptr))
		, m_device(std::exchange(other.m_device, nullptr))
		, m_swap_chain(std::exchange(other.m_swap_chain, nullptr))
	{
	}

	RenderContext &RenderContext::operator=(RenderContext &&other) noexcept
	{
		m_instance = std::exchange(other.m_instance, nullptr);
		m_debug_messenger = std::exchange(other.m_debug_messenger, nullptr);
		m_surface = std::exchange(other.m_surface, nullptr);
		m_device = std::exchange(other.m_device, nullptr);
		m_swap_chain = std::exchange(other.m_swap_chain, nullptr);
		return *this;
	}

	Expected<void> RenderContext::initialize(
		bool request_validation_layers,
		const Window &window)
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			return Error("failed to initialize volk");
		}

		m_validation_layers_enabled =
			request_validation_layers && validation_layers_supported();

		const auto instance = create_instance();
		if (instance.is_error())
		{
			return instance.error();
		}

		if (m_validation_layers_enabled)
		{
			const auto debug_messenger = create_debug_messenger();
			if (debug_messenger.is_error())
			{
				return debug_messenger.error();
			}
		}

		auto surface = window.create_surface(m_instance);
		if (surface.is_error())
		{
			return surface.error();
		}

		m_surface = std::move(surface.value());

		auto device = Device::create(m_instance, m_surface);
		if (device.is_error())
		{
			return device.error();
		}

		m_device = std::move(device.value());

		auto swap_chain = SwapChain::create(
			m_surface.release_non_null(), m_device.release_non_null(), window);
		if (swap_chain.is_error())
		{
			return swap_chain.error();
		}

		m_swap_chain = std::move(swap_chain.value());

		return {};
	}

	Expected<void> RenderContext::create_instance()
	{
		const VkApplicationInfo application_info = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "HyperEditor",
			.applicationVersion = Utils::make_api_version(0, 1, 0, 0),
			.pEngineName = "HyperEngine",
			.engineVersion = Utils::make_api_version(0, 1, 0, 0),
			.apiVersion = Utils::make_api_version(0, 1, 2, 0),
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
			.pfnUserCallback = Utils::debug_callback,
			.pUserData = nullptr,
		};

		const size_t layer_count =
			m_validation_layers_enabled ? s_validation_layers.size() : 0;
		const char *const *layers =
			m_validation_layers_enabled ? s_validation_layers.data() : nullptr;
		const std::vector<const char *> extensions = get_required_extensions();
		const VkInstanceCreateInfo instance_create_info = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = m_validation_layers_enabled ? &debug_messenger_info : nullptr,
			.flags = 0,
			.pApplicationInfo = &application_info,
			.enabledLayerCount = static_cast<uint32_t>(layer_count),
			.ppEnabledLayerNames = layers,
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
		};

		VkInstance instance = nullptr;
		const auto result =
			vkCreateInstance(&instance_create_info, nullptr, &instance);
		if (result != VK_SUCCESS)
		{
			return Error("failed to create instance");
		}

		m_instance = NonNullOwnPtr<VkInstance>(
			instance,
			[](VkInstance handle)
			{
				vkDestroyInstance(handle, nullptr);
			});

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
			.pfnUserCallback = Utils::debug_callback,
			.pUserData = nullptr,
		};

		VkDebugUtilsMessengerEXT debug_messenger = nullptr;
		const auto result = vkCreateDebugUtilsMessengerEXT(
			m_instance, &debug_messenger_info, nullptr, &debug_messenger);
		if (result != VK_SUCCESS)
		{
			return Error("failed to create debug messenger");
		}

		m_debug_messenger = NonNullOwnPtr<VkDebugUtilsMessengerEXT>(
			debug_messenger,
			[this](VkDebugUtilsMessengerEXT handle)
			{
				vkDestroyDebugUtilsMessengerEXT(m_instance, handle, nullptr);
			});

		return {};
	}

	bool RenderContext::validation_layers_supported() const noexcept
	{
		uint32_t layer_count = 0;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
		if (layer_count == 0)
		{
			return false;
		}

		std::vector<VkLayerProperties> layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, layers.data());

		for (const char *validation_layer : s_validation_layers)
		{
			for (const VkLayerProperties &properties : layers)
			{
				if (strcmp(validation_layer, properties.layerName) == 0)
				{
					continue;
				}

				return true;
			}
		}

		return false;
	}

	std::vector<const char *> RenderContext::get_required_extensions() const
	{
		uint32_t required_extension_count = 0;
		const char **required_extensions =
			glfwGetRequiredInstanceExtensions(&required_extension_count);
		if (required_extension_count == 0)
		{
			return {};
		}

		std::vector<const char *> extensions(
			required_extensions, required_extensions + required_extension_count);
		if (m_validation_layers_enabled)
		{
			extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	Expected<NonNullOwnPtr<RenderContext>> RenderContext::create(
		bool request_validation_layers,
		const Window &window)
	{
		auto render_context = make_non_null_own<RenderContext>();
		const auto result =
			render_context->initialize(request_validation_layers, window);
		if (result.is_error())
		{
			return result.error();
		}

		return render_context;
	}
} // namespace HyperEngine
