/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Assertion.hpp>
#include <HyperPlatform/IWindow.hpp>
#include <HyperRendering/Vulkan/Context.hpp>
#include <HyperRendering/Vulkan/IPlatformContext.hpp>
#include <HyperRendering/Vulkan/Renderer.hpp>
#include <cstring>
#include <string>
#include <vulkan/vulkan.h>

namespace HyperRendering
{
	HYPERENGINE_API IContext* create_context()
	{
		return new Vulkan::CContext();
	}

	namespace Vulkan
	{
		static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT severity_flags,
			VkDebugUtilsMessageTypeFlagsEXT,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void*)
		{
			if (severity_flags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				HyperCore::CLogger::fatal("Vulkan: {}", callback_data->pMessage);
			}
			else if (severity_flags >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				HyperCore::CLogger::warning("Vulkan: {}", callback_data->pMessage);
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

#if HYPERENGINE_DEBUG
			if (!setup_debug_messenger())
			{
				return false;
			}
#endif

			if (!m_swap_chain.initialize(this))
			{
				return false;
			}

			if (!m_device.initialize(this))
			{
				return false;
			}

			m_renderer = new CRenderer();

			return true;
		}

		void CContext::shutdown()
		{
			m_device.shutdown();
			m_swap_chain.shutdown();

#if HYPERENGINE_DEBUG
			if (m_validation_layer_support)
			{
				PFN_vkVoidFunction vkDestroyDebugUtilsMessengerEXTFunction =
					vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
				PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT =
					reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkDestroyDebugUtilsMessengerEXTFunction);

				vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
			}
#endif
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

			std::vector<const char*> extensions = { "VK_KHR_surface", m_platform_context->get_required_extension() };

			if (m_validation_layer_support)
			{
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			}

			uint32_t layer_count = 0;
			const char* const* layers = nullptr;
			const void* next = nullptr;

			VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
			if (m_validation_layer_support)
			{
				layer_count = static_cast<uint32_t>(s_validation_layers.size());
				layers = s_validation_layers.data();

				debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				debug_messenger_create_info.messageSeverity =
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				debug_messenger_create_info.messageType =
					VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				debug_messenger_create_info.pfnUserCallback = debug_callback;

				next = reinterpret_cast<const void*>(&debug_messenger_create_info);
			}

			VkInstanceCreateInfo instance_create_info{};
			instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instance_create_info.pApplicationInfo = &application_info;
			instance_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			instance_create_info.ppEnabledExtensionNames = extensions.data();
			instance_create_info.enabledLayerCount = layer_count;
			instance_create_info.ppEnabledLayerNames = layers;
			instance_create_info.pNext = next;

			if (vkCreateInstance(&instance_create_info, nullptr, &m_instance) != VK_SUCCESS)
			{
				HyperCore::CLogger::fatal("Vulkan: failed to create instance!");
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
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debug_messenger_create_info.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debug_messenger_create_info.pfnUserCallback = debug_callback;

			PFN_vkVoidFunction vkCreateDebugUtilsMessengerEXTFunction =
				vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
			PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT =
				reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkCreateDebugUtilsMessengerEXTFunction);

			if (vkCreateDebugUtilsMessengerEXT(m_instance, &debug_messenger_create_info, nullptr, &m_debug_messenger) !=
				VK_SUCCESS)
			{
				HyperCore::CLogger::fatal("Vulkan: failed to setup debug messenger!");
				return false;
			}

			return true;
		}

		bool CContext::is_validation_layer_available()
		{
			uint32_t available_layer_properties_count = 0;
			vkEnumerateInstanceLayerProperties(&available_layer_properties_count, nullptr);

			std::vector<VkLayerProperties> available_layer_properties(available_layer_properties_count);
			vkEnumerateInstanceLayerProperties(&available_layer_properties_count, available_layer_properties.data());

			for (const char* layer_name : s_validation_layers)
			{
				bool layer_found = false;

				for (const VkLayerProperties& layer_properties : available_layer_properties)
				{
					if (strcmp(layer_name, layer_properties.layerName) != 0)
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

		bool CContext::is_validation_layer_enabled() const
		{
			return m_validation_layer_support;
		}

		IRenderer& CContext::renderer()
		{
			return *m_renderer;
		}

		const IPlatformContext* CContext::platform_context() const
		{
			return m_platform_context;
		}

		const VkInstance& CContext::instance() const
		{
			return m_instance;
		}
		const CSwapChain& CContext::swap_chain() const
		{
			return m_swap_chain;
		}

		const CDevice& CContext::device() const
		{
			return m_device;
		}
	} // namespace Vulkan
} // namespace HyperRendering
