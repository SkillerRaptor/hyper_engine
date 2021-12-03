/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Device.hpp"

#include <vector>
#include <volk.h>

namespace HyperEngine::Rendering
{
	Device::Device(VkInstance instance, Error &error)
		: m_instance(instance)
	{
		auto physical_device = pick_physical_device();
		if (physical_device.is_error())
		{
			error = physical_device.error();
			return;
		}

		auto device = create_device();
		if (device.is_error())
		{
			error = device.error();
			return;
		}
	}

	Device::~Device()
	{
		if (m_device != nullptr)
		{
			vkDestroyDevice(m_device, nullptr);
		}
	}

	Device::Device(Device &&other) noexcept
	{
		m_physical_device = other.m_physical_device;
		m_device = other.m_device;

		other.m_physical_device = nullptr;
		other.m_device = nullptr;
	}

	Device &Device::operator=(Device &&other) noexcept
	{
		m_physical_device = other.m_physical_device;
		m_device = other.m_device;

		other.m_physical_device = nullptr;
		other.m_device = nullptr;

		return *this;
	}

	Expected<void> Device::pick_physical_device()
	{
		uint32_t physical_device_count = 0;
		vkEnumeratePhysicalDevices(m_instance, &physical_device_count, nullptr);
		if (physical_device_count == 0)
		{
			return Error("failed to find gpu with vulkan support");
		}

		std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
		vkEnumeratePhysicalDevices(
			m_instance, &physical_device_count, physical_devices.data());

		for (const VkPhysicalDevice &physical_device : physical_devices)
		{
			if (!is_physical_device_suitable(physical_device))
			{
				continue;
			}

			m_physical_device = physical_device;
			break;
		}

		if (m_physical_device == nullptr)
		{
			return Error("failed to find suitable gpu");
		}

		return {};
	}

	Expected<void> Device::create_device()
	{
		const Device::QueueFamilies queue_families =
			find_queue_families(m_physical_device);
		const float queue_priority = 1.0F;
		const VkDeviceQueueCreateInfo device_queue_create_info = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = queue_families.graphics_family.value(),
			.queueCount = 1,
			.pQueuePriorities = &queue_priority,
		};

		const VkPhysicalDeviceFeatures physical_device_features = {};
		const VkDeviceCreateInfo device_create_info = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &device_queue_create_info,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = 0,
			.ppEnabledExtensionNames = nullptr,
			.pEnabledFeatures = &physical_device_features,
		};

		const VkResult device_result = vkCreateDevice(
			m_physical_device, &device_create_info, nullptr, &m_device);
		if (device_result != VK_SUCCESS)
		{
			return Error("failed to create logical device");
		}

		vkGetDeviceQueue(
			m_device, queue_families.graphics_family.value(), 0, &m_graphics_queue);
		if (m_graphics_queue == nullptr)
		{
			return Error("failed to retrieve graphics family queue");
		}

		return {};
	}

	bool Device::is_physical_device_suitable(
		VkPhysicalDevice physical_device) const
	{
		const Device::QueueFamilies queue_families =
			find_queue_families(physical_device);
		return queue_families.graphics_family.has_value();
	}

	Expected<Device *> Device::create(VkInstance instance)
	{
		assert(instance != nullptr);

		Error error = Error::success();
		auto *device = new Device(instance, error);
		if (error.is_error())
		{
			delete device;
			return error;
		}

		return device;
	}

	Device::QueueFamilies Device::find_queue_families(
		VkPhysicalDevice physical_device) const
	{
		uint32_t queue_family_property_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(
			physical_device, &queue_family_property_count, nullptr);
		if (queue_family_property_count == 0)
		{
			return {};
		}

		std::vector<VkQueueFamilyProperties> queue_family_properties(
			queue_family_property_count);
		vkGetPhysicalDeviceQueueFamilyProperties(
			physical_device,
			&queue_family_property_count,
			queue_family_properties.data());

		Device::QueueFamilies queue_families = {};

		uint32_t i = 0;
		for (const VkQueueFamilyProperties &queue_family : queue_family_properties)
		{
			if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queue_families.graphics_family = i;
			}

			if (queue_families.graphics_family.has_value())
			{
				break;
			}

			++i;
		}

		return queue_families;
	}
} // namespace HyperEngine::Rendering
