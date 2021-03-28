#include "VulkanDevice.hpp"

#include <HyperCore/HyperAssert.hpp>

#include <vulkan/vulkan.h>

#include <vector>

#if HYPERENGINE_BUILD_VULKAN
	namespace HyperEngine
	{
		bool VulkanDevice::Initialize(VkInstance instance)
		{
			uint32_t physicalDeviceCount{ 0 };
			vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
			if (physicalDeviceCount == 0)
			{
				HYPERENGINE_ASSERT(false, "Failed to find a GPU with vulkan support!");
				return false;
			}
			
			std::vector<VkPhysicalDevice> physicalDevices{ physicalDeviceCount };
			vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
			
			VkPhysicalDevice bestPhysicalDevice{ nullptr };
			uint32_t bestScore{ 0 };
			for (const VkPhysicalDevice& physicalDevice : physicalDevices)
			{
				uint32_t score = RateDeviceSuitability(physicalDevice);
				if (score >= bestScore)
				{
					bestPhysicalDevice = physicalDevice;
					bestScore = score;
				}
			}
			
			if (bestPhysicalDevice == nullptr)
			{
				HYPERENGINE_ASSERT(false, "Failed to find a suitable GPU!");
				return false;
			}
			
			m_physicalDevice = bestPhysicalDevice;
			
			return true;
		}
		
		void VulkanDevice::Terminate()
		{
		
		}
		
		uint32_t VulkanDevice::RateDeviceSuitability(VkPhysicalDevice physicalDevice)
		{
			VkPhysicalDeviceProperties physicalDeviceProperties{};
			vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
			
			VkPhysicalDeviceFeatures physicalDeviceFeatures{};
			vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
		
			uint32_t score{ 0 };
			
			if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				score += 1000;
			}
			
			score += physicalDeviceProperties.limits.maxImageDimension2D;
			
			if (!physicalDeviceFeatures.geometryShader)
			{
				return 0;
			}
			
			QueueFamilyIndices queueFamilyIndices{ FindQueueFamilies(physicalDevice) };
			if (!queueFamilyIndices.IsComplete())
			{
				return 0;
			}
			
			return score;
		}
		
		VulkanDevice::QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice physicalDevice)
		{
			QueueFamilyIndices queueFamilyIndices{};
			
			uint32_t queueFamilyCount{ 0 };
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
			
			std::vector<VkQueueFamilyProperties> queueFamilyProperties{ queueFamilyCount };
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
			
			uint32_t i{ 0 };
			for (const VkQueueFamilyProperties& queueFamilyProperty : queueFamilyProperties)
			{
				if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					queueFamilyIndices.graphicsFamily = i;
				}
				
				if (queueFamilyIndices.IsComplete())
				{
					break;
				}
				
				i++;
			}
			
			return queueFamilyIndices;
		}
	}
#endif