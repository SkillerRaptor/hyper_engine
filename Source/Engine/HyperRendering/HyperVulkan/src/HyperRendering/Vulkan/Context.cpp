/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Logger.hpp>
#include <HyperPlatform/IWindow.hpp>
#include <HyperRendering/Vulkan/Context.hpp>
#include <HyperRendering/Vulkan/IPlatformContext.hpp>
#include <vulkan/vulkan.h>

namespace HyperRendering
{
	HYPERENGINE_API IContext* create_context()
	{
		return new Vulkan::CContext();
	}
	
	namespace Vulkan
	{
		bool CContext::initialize(HyperPlatform::IWindow* window)
		{
			m_platform_context = IPlatformContext::construct();
			m_platform_context->initialize(window);
			
			if (!create_instance())
			{
				return false;
			}
			
			return true;
		}
		
		void CContext::shutdown()
		{
			vkDestroyInstance(m_instance, nullptr);
		}
		
		void CContext::update()
		{
		}
		
		bool CContext::create_instance()
		{
			VkApplicationInfo applicationInfo{};
			applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			applicationInfo.pApplicationName = "HyperEngine";
			applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.pEngineName = "HyperEngine";
			applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.apiVersion = VK_API_VERSION_1_2;
			
			VkInstanceCreateInfo instanceCreateInfo{};
			instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfo.pApplicationInfo = &applicationInfo;
			
			const char* szSurfaceExtension[] = { "VK_KHR_surface", m_platform_context->get_required_extension() };
			instanceCreateInfo.enabledExtensionCount = 2;
			instanceCreateInfo.ppEnabledExtensionNames = szSurfaceExtension;
			
			if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS)
			{
				HyperCore::CLogger::fatal("Failed to create vulkan instance!");
				return false;
			}
			
			return true;
		}
	}
}
