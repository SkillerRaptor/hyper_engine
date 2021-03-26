#pragma once

#if HYPERENGINE_BUILD_VULKAN
	#include <HyperRendering/Context.hpp>
	
	#include <cstdint>
	
	struct VkInstance_T;
	typedef struct VkInstance_T* VkInstance;
	
	namespace HyperEngine
	{
		class VulkanContext : public Context
		{
		public:
			virtual void SetWindowHints() override;
			
			virtual void Initialize(GLFWwindow* pWindow) override;
			virtual void Terminate() override;
			
			virtual void Present() override;
		
		private:
			static bool IsValidationLayerAvailable();
			static void GetRequiredExtensions(const char**& extensions, uint32_t& extensionCount);
		
		private:
			bool m_isValidationLayerSupported{ false };
			VkInstance m_instance{ nullptr };
		};
	}
#endif