/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/Context.hpp"

#include "HyperEngine/Core/Logger.hpp"

#include <GLFW/glfw3.h>

namespace HyperEngine::Vulkan
{
	CContext::~CContext()
	{
		if (m_instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(m_instance, nullptr);
		}
	}
	
	auto CContext::create(const CContext::SDescription& description) -> bool
	{
		(void) description;
		
		if (volkInitialize() != VK_SUCCESS)
		{
			CLogger::fatal("CContext::create(): Failed to initialize volk");
			return false;
		}

		if (!create_instance())
		{
			CLogger::fatal("CContext::create(): Failed to create vulkan instance");
			return false;
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

		std::vector<const char*> required_extensions = request_required_extensions();
		
		VkInstanceCreateInfo instance_create_info{};
		instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_create_info.pNext = nullptr;
		instance_create_info.flags = 0;
		instance_create_info.pApplicationInfo = &application_info;
		instance_create_info.enabledLayerCount = 0;
		instance_create_info.ppEnabledLayerNames = nullptr;
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
	
	auto CContext::request_required_extensions() -> std::vector<const char*>
	{
		uint32_t required_instance_extension_count = 0;
		const char** required_instance_extensions = glfwGetRequiredInstanceExtensions(&required_instance_extension_count);
		
		std::vector<const char*> required_extensions(required_instance_extensions, required_instance_extensions + required_instance_extension_count);

		// TODO(SkillerRaptor): Add debug extension
		
		return required_extensions;
	}
} // namespace HyperEngine::Vulkan
