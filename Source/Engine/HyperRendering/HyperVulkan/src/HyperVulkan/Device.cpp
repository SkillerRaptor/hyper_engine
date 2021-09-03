/*
 * Copyright (c) 2020-$YEAR, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/Device.hpp"

#include <HyperCore/Logger.hpp>

#include <vector>

namespace HyperRendering::HyperVulkan
{
	Device::Device(VkInstance& t_instance)
		: m_instance(t_instance)
	{
	}

	auto Device::initialize() -> bool
	{
		if (!pick_physical_device())
		{
			HyperCore::Logger::fatal("Device::initialize(): Failed to pick physical vulkan device");
			return false;
		}

		if (!create_device())
		{
			HyperCore::Logger::fatal("Device::initialize(): Failed to create vulkan device");
			return false;
		}

		return true;
	}
	
	auto Device::terminate() -> bool
	{
		auto destroy_device = [this]() -> bool
		{
			if (m_device == VK_NULL_HANDLE)
			{
				return false;
			}
			
			vkDestroyDevice(m_device, nullptr);
			HyperCore::Logger::debug("Vulkan device was destroyed");
			
			return true;
		};

		if (!destroy_device())
		{
			return false;
		}
		
		return true;
	}

	auto Device::pick_physical_device() -> bool
	{
		uint32_t available_physical_device_count = 0;
		vkEnumeratePhysicalDevices(m_instance, &available_physical_device_count, nullptr);

		if (available_physical_device_count == 0)
		{
			HyperCore::Logger::fatal("Device::pick_physical_device(): Failed to find a physical device with Vulkan support");
			return false;
		}

		std::vector<VkPhysicalDevice> available_physical_devices(available_physical_device_count);
		vkEnumeratePhysicalDevices(m_instance, &available_physical_device_count, available_physical_devices.data());

		for (const auto& physical_device : available_physical_devices)
		{
			if (is_physical_device_suitable(physical_device))
			{
				m_physical_device = physical_device;
				break;
			}
		}

		if (m_physical_device == VK_NULL_HANDLE)
		{
			HyperCore::Logger::fatal("Device::pick_physical_device(): Failed to find a suitable physical vulkan device");
			return false;
		}

		HyperCore::Logger::debug("Physical vulkan device was found");

		return true;
	}

	auto Device::is_physical_device_suitable(VkPhysicalDevice physical_device) const -> bool
	{
		auto queue_family_indices = find_queue_families(physical_device);
		return queue_family_indices.is_complete();
	}

	auto Device::create_device() -> bool
	{
		auto queue_family_indices = find_queue_families(m_physical_device);
		auto queue_priority = 1.0F;
		
		VkDeviceQueueCreateInfo device_queue_create_info{};
		device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		device_queue_create_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
		device_queue_create_info.pQueuePriorities = &queue_priority;
		device_queue_create_info.queueCount = 1;

		VkPhysicalDeviceFeatures physical_device_features{};

		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.pEnabledFeatures = &physical_device_features;
		device_create_info.pQueueCreateInfos = &device_queue_create_info;
		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(1);
		device_create_info.ppEnabledExtensionNames = s_device_extensions.data();
		device_create_info.enabledExtensionCount = static_cast<uint32_t>(s_device_extensions.size());

		if (vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("Device::create_device(): Failed to create Vulkan device");
			return false;
		}

		vkGetDeviceQueue(m_device, queue_family_indices.graphics_family.value(), 0, &m_graphics_queue);

		volkLoadDevice(m_device);

		HyperCore::Logger::debug("Vulkan device was created");

		return true;
	}

	auto Device::find_queue_families(VkPhysicalDevice physical_device) const -> Device::QueueFamilyIndices
	{
		Device::QueueFamilyIndices queue_family_indices;

		uint32_t available_queue_family_properties_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &available_queue_family_properties_count, nullptr);

		std::vector<VkQueueFamilyProperties> available_queue_family_properties(available_queue_family_properties_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &available_queue_family_properties_count, available_queue_family_properties.data());

		uint32_t i = 0;
		for (const auto& queue_family_properties : available_queue_family_properties)
		{
			if (queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queue_family_indices.graphics_family = i;
			}

			if (queue_family_indices.is_complete())
			{
				break;
			}

			++i;
		}

		return queue_family_indices;
	}

	auto Device::QueueFamilyIndices::is_complete() const -> bool
	{
		return graphics_family.has_value();
	}
} // namespace HyperRendering::HyperVulkan
