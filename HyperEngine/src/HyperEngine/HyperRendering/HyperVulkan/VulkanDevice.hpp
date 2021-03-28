#pragma once


#if HYPERENGINE_BUILD_VULKAN
	#include <cstdint>
	
	struct VkInstance_T;
	typedef struct VkInstance_T* VkInstance;

	struct VkPhysicalDevice_T;
	typedef struct VkPhysicalDevice_T* VkPhysicalDevice;

	namespace HyperEngine
	{
		class VulkanDevice
		{
		public:
			bool Initialize(VkInstance instance);
			void Terminate();
		
		private:
			uint32_t RateDeviceSuitability(VkPhysicalDevice physicalDevice);
			
		private:
			VkPhysicalDevice m_physicalDevice{ nullptr };
		};
	}
#endif