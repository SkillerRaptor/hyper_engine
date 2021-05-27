#include <vulkan/context.hpp>

#include <core/logger.hpp>

#include <vulkan/vulkan.h>

namespace rendering
{
	HYPERENGINE_API context* create_context()
	{
		return new vulkan::context{};
	}
	
	namespace vulkan
	{
		bool context::initialize(void* instance, void* native_window)
		{
			m_platform_context = platform_context::construct();
			m_platform_context->initialize(instance, native_window);
			
			if (!create_instance())
			{
				return false;
			}
			
			return true;
		}
		
		void context::shutdown()
		{
			vkDestroyInstance(m_instance, nullptr);
		}
		
		void context::update()
		{
		}
		
		bool context::create_instance()
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
				core::logger::fatal("Failed to create vulkan instance!");
				return false;
			}
			
			return true;
		}
	}
}
