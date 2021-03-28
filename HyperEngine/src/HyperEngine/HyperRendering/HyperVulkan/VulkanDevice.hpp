#pragma once


#if HYPERENGINE_BUILD_VULKAN
	#include <optional>
	
	struct VkInstance_T;
	typedef struct VkInstance_T* VkInstance;

	struct VkPhysicalDevice_T;
	typedef struct VkPhysicalDevice_T* VkPhysicalDevice;

	namespace HyperEngine
	{
		class VulkanDevice
		{
		private:
			struct QueueFamilyIndices
			{
				std::optional<uint32_t> graphicsFamily;
				
				bool IsComplete() const
				{
					return graphicsFamily.has_value();
				}
			};
			
		public:
			bool Initialize(VkInstance instance);
			void Terminate();
		
		private:
			static uint32_t RateDeviceSuitability(VkPhysicalDevice physicalDevice);
			static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);
			
		private:
			VkPhysicalDevice m_physicalDevice{ nullptr };
		};
	}
#endif