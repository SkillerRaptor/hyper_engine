/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/Device.hpp"

#include "HyperEngine/Core/Logger.hpp"

#include <set>
#include <vector>

namespace HyperEngine::Vulkan
{
	auto CDevice::destroy() -> void
	{
		if (m_device != VK_NULL_HANDLE)
		{
			vkDestroyDevice(m_device, nullptr);
		}
	}

	auto CDevice::create(const CDevice::SDescription& description) -> bool
	{
		if (description.instance == VK_NULL_HANDLE)
		{
			CLogger::fatal("CDevice::create(): The description vulkan instance is null");
			return false;
		}

		if (description.surface == VK_NULL_HANDLE)
		{
			CLogger::fatal("CDevice::create(): The description vulkan surface is null");
			return false;
		}

		m_instance = description.instance;
		m_surface = description.surface;

		if (!select_physical_device())
		{
			CLogger::fatal("CDevice::create(): Failed to select vulkan physical device");
			return false;
		}

		if (!create_logical_device())
		{
			CLogger::fatal("CDevice::create(): Failed to create vulkan logical device");
			return false;
		}

		return true;
	}

	auto CDevice::select_physical_device() -> bool
	{
		uint32_t available_physical_device_count = 0;
		vkEnumeratePhysicalDevices(m_instance, &available_physical_device_count, nullptr);

		if (available_physical_device_count == 0)
		{
			CLogger::fatal("CDevice::select_physical_device(): Failed to find physical device with vulkan");
			return false;
		}

		std::vector<VkPhysicalDevice> available_physical_devices(available_physical_device_count);
		vkEnumeratePhysicalDevices(m_instance, &available_physical_device_count, available_physical_devices.data());

		for (const VkPhysicalDevice& physical_device : available_physical_devices)
		{
			if (is_physical_device_suitable(physical_device))
			{
				m_physical_device = physical_device;
				break;
			}
		}

		if (m_physical_device == VK_NULL_HANDLE)
		{
			CLogger::fatal("CDevice::select_physical_device(): Failed to find suitable physical vulkan device");
			return false;
		}

		return true;
	}

	auto CDevice::create_logical_device() -> bool
	{
		const SQueueFamilies queue_families = find_queue_families(m_physical_device);

		std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos{};

		const float queue_priority = 1.0F;
		const std::set<uint32_t> unique_queue_families = {
			queue_families.graphics_family.value(),
			queue_families.present_family.value()
		};
		
		for (uint32_t queue_family : unique_queue_families)
		{
			VkDeviceQueueCreateInfo device_queue_create_info{};
			device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			device_queue_create_info.pNext = nullptr;
			device_queue_create_info.flags = 0;
			device_queue_create_info.queueFamilyIndex = queue_family;
			device_queue_create_info.queueCount = 1;
			device_queue_create_info.pQueuePriorities = &queue_priority;
			
			device_queue_create_infos.emplace_back(device_queue_create_info);
		}

		VkPhysicalDeviceFeatures physical_device_features{};
		physical_device_features.robustBufferAccess = VK_FALSE;
		physical_device_features.fullDrawIndexUint32 = VK_FALSE;
		physical_device_features.imageCubeArray = VK_FALSE;
		physical_device_features.independentBlend = VK_FALSE;
		physical_device_features.geometryShader = VK_FALSE;
		physical_device_features.tessellationShader = VK_FALSE;
		physical_device_features.sampleRateShading = VK_FALSE;
		physical_device_features.dualSrcBlend = VK_FALSE;
		physical_device_features.logicOp = VK_FALSE;
		physical_device_features.multiDrawIndirect = VK_FALSE;
		physical_device_features.drawIndirectFirstInstance = VK_FALSE;
		physical_device_features.depthClamp = VK_FALSE;
		physical_device_features.depthBiasClamp = VK_FALSE;
		physical_device_features.fillModeNonSolid = VK_FALSE;
		physical_device_features.depthBounds = VK_FALSE;
		physical_device_features.wideLines = VK_FALSE;
		physical_device_features.largePoints = VK_FALSE;
		physical_device_features.alphaToOne = VK_FALSE;
		physical_device_features.multiViewport = VK_FALSE;
		physical_device_features.samplerAnisotropy = VK_FALSE;
		physical_device_features.textureCompressionETC2 = VK_FALSE;
		physical_device_features.textureCompressionASTC_LDR = VK_FALSE;
		physical_device_features.textureCompressionBC = VK_FALSE;
		physical_device_features.occlusionQueryPrecise = VK_FALSE;
		physical_device_features.pipelineStatisticsQuery = VK_FALSE;
		physical_device_features.vertexPipelineStoresAndAtomics = VK_FALSE;
		physical_device_features.fragmentStoresAndAtomics = VK_FALSE;
		physical_device_features.shaderTessellationAndGeometryPointSize = VK_FALSE;
		physical_device_features.shaderImageGatherExtended = VK_FALSE;
		physical_device_features.shaderStorageImageExtendedFormats = VK_FALSE;
		physical_device_features.shaderStorageImageMultisample = VK_FALSE;
		physical_device_features.shaderStorageImageReadWithoutFormat = VK_FALSE;
		physical_device_features.shaderStorageImageWriteWithoutFormat = VK_FALSE;
		physical_device_features.shaderUniformBufferArrayDynamicIndexing = VK_FALSE;
		physical_device_features.shaderSampledImageArrayDynamicIndexing = VK_FALSE;
		physical_device_features.shaderStorageBufferArrayDynamicIndexing = VK_FALSE;
		physical_device_features.shaderStorageImageArrayDynamicIndexing = VK_FALSE;
		physical_device_features.shaderClipDistance = VK_FALSE;
		physical_device_features.shaderCullDistance = VK_FALSE;
		physical_device_features.shaderFloat64 = VK_FALSE;
		physical_device_features.shaderInt64 = VK_FALSE;
		physical_device_features.shaderInt16 = VK_FALSE;
		physical_device_features.shaderResourceResidency = VK_FALSE;
		physical_device_features.shaderResourceMinLod = VK_FALSE;
		physical_device_features.sparseBinding = VK_FALSE;
		physical_device_features.sparseResidencyBuffer = VK_FALSE;
		physical_device_features.sparseResidencyImage2D = VK_FALSE;
		physical_device_features.sparseResidencyImage3D = VK_FALSE;
		physical_device_features.sparseResidency2Samples = VK_FALSE;
		physical_device_features.sparseResidency4Samples = VK_FALSE;
		physical_device_features.sparseResidency8Samples = VK_FALSE;
		physical_device_features.sparseResidency16Samples = VK_FALSE;
		physical_device_features.sparseResidencyAliased = VK_FALSE;
		physical_device_features.variableMultisampleRate = VK_FALSE;
		physical_device_features.inheritedQueries = VK_FALSE;

		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.pNext = nullptr;
		device_create_info.flags = 0;
		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(device_queue_create_infos.size());
		device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
		device_create_info.enabledLayerCount = 0;
		device_create_info.ppEnabledLayerNames = nullptr;
		device_create_info.enabledExtensionCount = 0;
		device_create_info.ppEnabledExtensionNames = nullptr;
		device_create_info.pEnabledFeatures = &physical_device_features;

		if (vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device) != VK_SUCCESS)
		{
			CLogger::fatal("CDevice::create_logical_device(): Failed to create vulkan logical device");
			return false;
		}

		vkGetDeviceQueue(m_device, queue_families.graphics_family.value(), 0, &m_queues.graphics_queue);
		vkGetDeviceQueue(m_device, queue_families.present_family.value(), 0, &m_queues.present_queue);

		return true;
	}

	auto CDevice::is_physical_device_suitable(VkPhysicalDevice physical_device) const -> bool
	{
		const CDevice::SQueueFamilies queue_families = find_queue_families(physical_device);

		return queue_families.graphics_family.has_value() && queue_families.present_family.has_value();
	}

	auto CDevice::find_queue_families(VkPhysicalDevice physical_device) const -> CDevice::SQueueFamilies
	{
		uint32_t available_queue_family_properties_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &available_queue_family_properties_count, nullptr);

		if (available_queue_family_properties_count == 0)
		{
			return {};
		}

		std::vector<VkQueueFamilyProperties> available_queue_family_properties(available_queue_family_properties_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &available_queue_family_properties_count, available_queue_family_properties.data());

		CDevice::SQueueFamilies queue_families{};

		uint32_t i = 0;
		for (const VkQueueFamilyProperties& queue_family_properties : available_queue_family_properties)
		{
			if (queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queue_families.graphics_family = i;
			}

			VkBool32 present_queue_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, m_surface, &present_queue_support);

			if (present_queue_support)
			{
				queue_families.present_family = i;
			}

			if (queue_families.graphics_family.has_value() && queue_families.present_family.has_value())
			{
				return queue_families;
			}

			++i;
		}

		return queue_families;
	}
} // namespace HyperEngine::Vulkan
