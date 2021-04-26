#pragma once

#if HYPERENGINE_BUILD_VULKAN
	#include <cstdint>
	#include <optional>
	#include <vector>
	
	struct VkInstance_T;
	typedef struct VkInstance_T* VkInstance;
	
	struct VkPhysicalDevice_T;
	typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
	
	struct VkDevice_T;
	typedef struct VkDevice_T* VkDevice;
	
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
			bool Initialize(
				VkInstance instance,
				bool isValidationLayerSupported,
				const std::vector<const char*>& validationLayers);
			void Terminate();
		
		private:
			bool PickPhysicalDevice(VkInstance instance);
			bool CreateLogicalDevice(
				bool isValidationLayerSupported,
				const std::vector<const char*>& validationLayers);
			
			static uint32_t RateDeviceSuitability(VkPhysicalDevice physicalDevice);
			static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);
		
		private:
			VkPhysicalDevice m_physicalDevice{ nullptr };
			VkDevice m_logicalDevice{ nullptr };
		};
	}
#endif