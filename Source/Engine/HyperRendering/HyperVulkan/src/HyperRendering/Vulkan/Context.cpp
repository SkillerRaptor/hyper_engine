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
		const std::vector<const char*> CContext::s_validation_layers = {
			"VK_LAYER_KHRONOS_validation"
		};
		
		static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT severity,
			VkDebugUtilsMessageTypeFlagsEXT type,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void* user_data)
		{
			if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				HyperCore::CLogger::fatal("Vulkan Validation Error - {}", callback_data->pMessage);
			}
			else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				HyperCore::CLogger::warning("Vulkan Validation Warning - {}", callback_data->pMessage);
			}
			
			return VK_FALSE;
		}
		
		bool CContext::initialize(HyperPlatform::IWindow* window)
		{
			m_platform_context = IPlatformContext::construct();
			m_platform_context->initialize(window);

#if HYPERENGINE_DEBUG
			if (is_validation_layer_available())
			{
				m_validation_layer_support = true;
			}
#endif
			
			if (!create_instance())
			{
				return false;
			}
			
			if (!setup_debug_messenger())
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
			VkApplicationInfo application_info{};
			application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			application_info.pApplicationName = "HyperEngine";
			application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			application_info.pEngineName = "HyperEngine";
			application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			application_info.apiVersion = VK_API_VERSION_1_2;
			
			VkInstanceCreateInfo instance_create_info{};
			instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instance_create_info.pApplicationInfo = &application_info;
			
			const char* surface_extensions[] = {
				"VK_KHR_surface",
				"VK_EXT_DEBUG_UTILS_EXTENSION_NAME",
				m_platform_context->get_required_extension()
			};
			
			instance_create_info.enabledExtensionCount = 3;
			instance_create_info.ppEnabledExtensionNames = surface_extensions;
			
			instance_create_info.enabledLayerCount = 0;
			instance_create_info.ppEnabledLayerNames = nullptr;
			
			VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
			if (m_validation_layer_support)
			{
				instance_create_info.ppEnabledLayerNames = s_validation_layers.data();
				instance_create_info.enabledLayerCount = static_cast<uint32_t>(s_validation_layers.size());
				
				debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				debug_messenger_create_info.messageSeverity =
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				debug_messenger_create_info.messageType =
					VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				debug_messenger_create_info.pfnUserCallback = debug_callback;
				debug_messenger_create_info.pUserData = nullptr;
				
				instance_create_info.pNext = reinterpret_cast<const void*>(&debug_messenger_create_info);
			}
			
			if (vkCreateInstance(&instance_create_info, nullptr, &m_instance) != VK_SUCCESS)
			{
				HyperCore::CLogger::fatal("Failed to create vulkan instance!");
				return false;
			}
			
			return true;
		}
		
		bool CContext::setup_debug_messenger()
		{
			if (!m_validation_layer_support)
			{
				return true;
			}
			
			VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
			debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debug_messenger_create_info.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debug_messenger_create_info.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debug_messenger_create_info.pfnUserCallback = debug_callback;
			debug_messenger_create_info.pUserData = nullptr;
			
			PFN_vkVoidFunction vkCreateDebugUtilsMessengerEXTFunction = vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
			PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkCreateDebugUtilsMessengerEXTFunction);
			
			if (vkCreateDebugUtilsMessengerEXT(m_instance, &debug_messenger_create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
			{
				HyperCore::CLogger::fatal("Failed to setup vulkan debug messenger!");
				return false;
			}
			
			return true;
		}
		
		bool CContext::is_validation_layer_available()
		{
			uint32_t layer_count = 0;
			vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
			
			std::vector<VkLayerProperties> available_layer_properties(layer_count);
			vkEnumerateInstanceLayerProperties(&layer_count, available_layer_properties.data());
			
			for (const char* layer_name : s_validation_layers)
			{
				bool layer_found = false;
				
				for (const VkLayerProperties& layer_properties : available_layer_properties)
				{
					if (std::strcmp(layer_name, layer_properties.layerName) != 0)
					{
						continue;
					}
					
					layer_found = true;
				}
				
				if (!layer_found)
				{
					return false;
				}
			}
			
			return true;
		}
	}
}
