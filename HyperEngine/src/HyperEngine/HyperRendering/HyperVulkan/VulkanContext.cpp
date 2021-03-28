#include "VulkanContext.hpp"

#if HYPERENGINE_BUILD_VULKAN
	#include <HyperCore/HyperAssert.hpp>
	
	#define GLFW_INCLUDE_VULKAN
	#include <GLFW/glfw3.h>
	
	#include <vector>
	
	namespace HyperEngine
	{
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT debugUtilsMessageSeverityFlagBitsEXT,
			VkDebugUtilsMessageTypeFlagsEXT debugUtilsMessageTypeFlagsEXT,
			const VkDebugUtilsMessengerCallbackDataEXT* pDebugUtilsMessengerCallbackDataEXT,
			void* pUserData);
	
		static const char* g_szValidationLayers[] =
		{
			"VK_LAYER_KHRONOS_validation"
		};
		
		void VulkanContext::SetWindowHints()
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}
		
		bool VulkanContext::Initialize(GLFWwindow* pWindow)
		{
			#if HYPERENGINE_DEBUG
				if (IsValidationLayerAvailable())
				{
					m_isValidationLayerSupported = true;
				}
			#endif
			
			if (!CreateInstance(pWindow))
			{
				return false;
			}
			
			if (!SetupDebugMessenger())
			{
				return false;
			}
			
			return true;
		}
		
		void VulkanContext::Terminate()
		{
			if (m_isValidationLayerSupported)
			{
				PFN_vkVoidFunction vkDestroyDebugUtilsMessengerEXTFunction{ vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT") };
				PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT{ reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkDestroyDebugUtilsMessengerEXTFunction) };
				vkDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
			}
			
			vkDestroyInstance(m_instance, nullptr);
		}
		
		void VulkanContext::Present()
		{
			glfwPollEvents();
		}
		
		bool VulkanContext::CreateInstance(GLFWwindow* pWindow)
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
			
			uint32_t extensionCount{ 0 };
			const char** extensions{ nullptr };
			GetRequiredExtensions(extensions, extensionCount);
			
			std::vector<const char*> requiredExtensions{ extensionCount };
			for (size_t i{ 0 }; i < static_cast<size_t>(extensionCount); i++)
			{
				requiredExtensions.emplace_back(extensions[i]);
			}
			
			requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			
			instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
			instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
			
			instanceCreateInfo.ppEnabledLayerNames = nullptr;
			instanceCreateInfo.enabledLayerCount = 0;
			
			if (m_isValidationLayerSupported)
			{
				instanceCreateInfo.ppEnabledLayerNames = g_szValidationLayers;
				instanceCreateInfo.enabledLayerCount = sizeof(g_szValidationLayers) / sizeof(g_szValidationLayers[0]);
				
				VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
				debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				debugMessengerCreateInfo.messageSeverity =
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				debugMessengerCreateInfo.messageType =
					VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				debugMessengerCreateInfo.pfnUserCallback = DebugCallback;
				debugMessengerCreateInfo.pUserData = nullptr;
				
				instanceCreateInfo.pNext = reinterpret_cast<const void*>(&debugMessengerCreateInfo);
			}
			
			if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS)
			{
				HYPERENGINE_ASSERT(false, "Failed to create vulkan instance!");
				return false;
			}
			
			return true;
		}
		
		bool VulkanContext::SetupDebugMessenger()
		{
			if (!m_isValidationLayerSupported)
			{
				return true;
			}
			
			VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
			debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugMessengerCreateInfo.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugMessengerCreateInfo.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugMessengerCreateInfo.pfnUserCallback = DebugCallback;
			debugMessengerCreateInfo.pUserData = nullptr;
			
			PFN_vkVoidFunction vkCreateDebugUtilsMessengerEXTFunction{ vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT") };
			PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT{ reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkCreateDebugUtilsMessengerEXTFunction) };
			
			if (vkCreateDebugUtilsMessengerEXT(m_instance, &debugMessengerCreateInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
			{
				HYPERENGINE_ASSERT(false, "Failed to setup debug messenger!");
				return false;
			}
			
			return true;
		}
		
		bool VulkanContext::IsValidationLayerAvailable()
		{
			uint32_t layerCount{ 0 };
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			
			std::vector<VkLayerProperties> availableLayers{ layerCount };
			vkEnumerateInstanceLayerProperties(&layerCount, &availableLayers[0]);
			
			for (const char* szLayerName : g_szValidationLayers)
			{
				bool wasLayerFound{ false };
				
				for (const VkLayerProperties& layerProperties : availableLayers)
				{
					if (std::strcmp(szLayerName, layerProperties.layerName) != 0)
					{
						continue;
					}
					
					wasLayerFound = true;
				}
				
				if (!wasLayerFound)
				{
					return false;
				}
			}
			
			return true;
		}
		
		void VulkanContext::GetRequiredExtensions(const char**& extensions, uint32_t& extensionCount)
		{
			extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
		}
		
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT severity,
			VkDebugUtilsMessageTypeFlagsEXT type,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData)
		{
			if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				HYPERENGINE_CORE_ERROR("{}", pCallbackData->pMessage);
				HYPERENGINE_DEBUGBREAK();
			}
			else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				HYPERENGINE_CORE_WARNING("{}", pCallbackData->pMessage);
			}
			else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			{
				HYPERENGINE_CORE_INFO("{}", pCallbackData->pMessage);
			}
			
			return VK_FALSE;
		}
	}
#endif