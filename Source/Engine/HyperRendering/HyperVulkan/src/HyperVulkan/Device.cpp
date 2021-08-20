/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/Device.hpp"

#include "HyperVulkan/Context.hpp"

#include <volk.h>

namespace HyperRendering::Vulkan
{
	Device::Device(Context& context)
		: m_context(context)
	{
	}

	auto Device::initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
	{
		auto physical_device_result = pick_physical_device();
		if (physical_device_result.is_error())
		{
			return physical_device_result;
		}
		
		auto device_result = create_device();
		if (device_result.is_error())
		{
			return device_result;
		}

		return {};
	}

	auto Device::terminate() -> void
	{
		vkDestroyDevice(m_device, nullptr);
		HyperCore::Logger::debug("Vulkan device was destroyed");
	}

	auto Device::pick_physical_device() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
	{
		uint32_t available_physical_device_count = 0;
		vkEnumeratePhysicalDevices(m_context.instance(), &available_physical_device_count, nullptr);

		if (available_physical_device_count == 0)
		{
			HyperCore::Logger::fatal("Failed to find a physical device with Vulkan support");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

		std::vector<VkPhysicalDevice> available_physical_devices(available_physical_device_count);
		vkEnumeratePhysicalDevices(m_context.instance(), &available_physical_device_count, available_physical_devices.data());

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
			HyperCore::Logger::fatal("Failed to find a suitable physical vulkan device");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

		HyperCore::Logger::debug("Physical vulkan device was found");

		return {};
	}

	auto Device::create_device() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
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
		device_create_info.pQueueCreateInfos = &device_queue_create_info;
		device_create_info.queueCreateInfoCount = 1;
		device_create_info.pEnabledFeatures = &physical_device_features;
		device_create_info.ppEnabledExtensionNames = nullptr;
		device_create_info.enabledExtensionCount = 0;

#if HYPERENGINE_DEBUG
		device_create_info.ppEnabledLayerNames = Context::validation_layers().data();
		device_create_info.enabledLayerCount = static_cast<uint32_t>(Context::validation_layers().size());
#endif

		if (vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("Failed to create Vulkan device");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

		vkGetDeviceQueue(m_device, queue_family_indices.graphics_family.value(), 0, &m_graphics_queue);
		
		HyperCore::Logger::debug("Vulkan device was created");

		return {};
	}

	auto Device::is_physical_device_suitable(VkPhysicalDevice physical_device) const -> bool
	{
		auto queue_family_indices = find_queue_families(physical_device);
		return queue_family_indices.is_complete();
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
} // namespace HyperRendering::Vulkan
