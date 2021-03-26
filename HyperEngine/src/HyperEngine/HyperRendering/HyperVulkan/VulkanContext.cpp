#include "VulkanContext.hpp"

#if HYPERENGINE_BUILD_VULKAN
	#include <HyperCore/HyperAssert.hpp>
	
	#define GLFW_INCLUDE_VULKAN
	#include <GLFW/glfw3.h>
	
	#include <vector>
	
	namespace HyperEngine
	{
		static const char* g_szValidationLayers[] =
		{
			"VK_LAYER_KHRONOS_validation"
		};
		
		void VulkanContext::SetWindowHints()
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}
		
		void VulkanContext::Initialize(GLFWwindow* pWindow)
		{
			#if HYPERENGINE_DEBUG
				if (IsValidationLayerAvailable())
				{
					m_isValidationLayerSupported = true;
				}
			#endif
			
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
			}
			
			if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS)
			{
				HYPERENGINE_ASSERT(false, "Failed to create vulkan instance!");
				glfwDestroyWindow(pWindow);
				glfwTerminate();
				std::exit(EXIT_FAILURE);
			}
		}
		
		void VulkanContext::Terminate()
		{
			vkDestroyInstance(m_instance, nullptr);
		}
		
		void VulkanContext::Present()
		{
			glfwPollEvents();
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
	}
#endif