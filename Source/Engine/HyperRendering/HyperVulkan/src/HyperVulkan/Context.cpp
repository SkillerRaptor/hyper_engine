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
	Context::Context(HyperPlatform::Window& window)
		: IContext(window)
	{
	}

	Context::~Context()
	{
		vkDestroyInstance(m_instance, nullptr);
	}
	
	auto Context::initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			HyperCore::Logger::error("Failed to initialize volk!");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

		VkApplicationInfo application_info{};
		application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		application_info.pApplicationName = m_window.title().c_str();
		application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		application_info.pEngineName = "HyperEngine";
		application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		application_info.apiVersion = VK_API_VERSION_1_2;
		
		uint32_t extension_count = 0;
		const char** extensions = nullptr;
		get_instance_extensions(extensions, extension_count);
		
		VkInstanceCreateInfo instance_create_info{};
		instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_create_info.pApplicationInfo = &application_info;
		instance_create_info.ppEnabledLayerNames = nullptr;
		instance_create_info.enabledLayerCount = 0;
		instance_create_info.ppEnabledExtensionNames = extensions;
		instance_create_info.enabledExtensionCount = extension_count;
		
		if (vkCreateInstance(&instance_create_info, nullptr, &m_instance) != VK_SUCCESS)
		{
			HyperCore::Logger::error("Failed to create VkInstance!");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

		volkLoadInstance(m_instance);

		return {};
	}

	auto Context::update() -> void
	{
	}
	
	auto Context::get_instance_extensions(const char**& extensions, uint32_t& extension_count) const -> void
	{
		extensions = glfwGetRequiredInstanceExtensions(&extension_count);
	}
} // namespace HyperRendering::Vulkan
