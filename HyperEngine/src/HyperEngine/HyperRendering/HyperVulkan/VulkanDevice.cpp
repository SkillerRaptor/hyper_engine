#include "VulkanDevice.hpp"

#include <HyperCore/HyperAssert.hpp>

#include <vulkan/vulkan.h>

#include <vector>

#if HYPERENGINE_BUILD_VULKAN
	namespace HyperEngine
	{
		bool VulkanDevice::Initialize(
			VkInstance instance,
			bool isValidationLayerSupported,
			const std::vector<const char*>& validationLayers)
		{
			if (!PickPhysicalDevice(instance))
			{
				return false;
			}
			
			if (!CreateLogicalDevice(isValidationLayerSupported, validationLayers))
			{
				return false;
			}
			
			return true;
		}
		
		void VulkanDevice::Terminate()
		{
			vkDestroyDevice(m_logicalDevice, nullptr);
		}
		
		bool VulkanDevice::PickPhysicalDevice(VkInstance instance)
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
		
		bool VulkanDevice::CreateLogicalDevice(
			bool isValidationLayerSupported,
			const std::vector<const char*>& validationLayers)
		{
			QueueFamilyIndices queueFamilyIndices{ FindQueueFamilies(m_physicalDevice) };
			
			VkDeviceQueueCreateInfo logicalDeviceQueueCreateInfo{};
			logicalDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			logicalDeviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
			logicalDeviceQueueCreateInfo.queueCount = 1;
			
			float logicalDeviceQueuePriority{ 1.0f };
			logicalDeviceQueueCreateInfo.pQueuePriorities = &logicalDeviceQueuePriority;
			
			VkPhysicalDeviceFeatures physicalDeviceFeatures{};
			
			VkDeviceCreateInfo logicalDeviceCreateInfo{};
			logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			logicalDeviceCreateInfo.pQueueCreateInfos = &logicalDeviceQueueCreateInfo;
			logicalDeviceCreateInfo.queueCreateInfoCount = 1;
			logicalDeviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
			logicalDeviceCreateInfo.ppEnabledExtensionNames = nullptr;
			logicalDeviceCreateInfo.enabledExtensionCount = 0;
			
			logicalDeviceCreateInfo.ppEnabledLayerNames = nullptr;
			logicalDeviceCreateInfo.enabledLayerCount = 0;
			
			if (isValidationLayerSupported)
			{
				logicalDeviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
				logicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			}
			
			if (vkCreateDevice(m_physicalDevice, &logicalDeviceCreateInfo, nullptr, &m_logicalDevice) != VK_SUCCESS)
			{
				HYPERENGINE_ASSERT(false, "Failed to create a logical device!");
				return false;
			}
			
			return true;
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