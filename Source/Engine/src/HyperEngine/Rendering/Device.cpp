/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Device.hpp"

#include <set>
#include <vector>
#include <volk.h>

namespace HyperEngine::Rendering
{
	Device::Device(VkInstance instance, VkSurfaceKHR surface, Error &error)
		: m_instance(instance)
		, m_surface(surface)
	{
		const Expected<void> physical_device = find_physical_device();
		if (physical_device.is_error())
		{
			error = physical_device.error();
			return;
		}

		const Expected<void> device = create_device();
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

	Expected<void> Device::find_physical_device()
	{
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
		if (device_count == 0)
		{
			return Error("failed to find gpu with vulkan support");
		}

		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

		for (const VkPhysicalDevice &device : devices)
		{
			if (!check_physical_device_suitability(device))
			{
				continue;
			}

			m_physical_device = device;
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

		std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos = {};

		const float queue_priority = 1.0F;
		const std::set<uint32_t> queue_family_list = {
			queue_families.graphics_family.value(),
			queue_families.present_family.value()
		};
		for (uint32_t queue_family : queue_family_list)
		{
			const VkDeviceQueueCreateInfo device_queue_create_info = {
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.queueFamilyIndex = queue_family,
				.queueCount = 1,
				.pQueuePriorities = &queue_priority,
			};
			device_queue_create_infos.push_back(device_queue_create_info);
		}

		const VkPhysicalDeviceFeatures physical_device_features = {};
		const VkDeviceCreateInfo device_create_info = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount =
				static_cast<uint32_t>(device_queue_create_infos.size()),
			.pQueueCreateInfos = device_queue_create_infos.data(),
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

		vkGetDeviceQueue(
			m_device, queue_families.present_family.value(), 0, &m_present_queue);
		if (m_present_queue == nullptr)
		{
			return Error("failed to retrieve present family queue");
		}

		return {};
	}

	bool Device::check_physical_device_suitability(
		VkPhysicalDevice physical_device) const
	{
		const Device::QueueFamilies queue_families =
			find_queue_families(physical_device);

		return queue_families.graphics_family.has_value() &&
					 queue_families.present_family.has_value();
	}

	Device::QueueFamilies Device::find_queue_families(
		VkPhysicalDevice physical_device) const
	{
		uint32_t property_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(
			physical_device, &property_count, nullptr);
		if (property_count == 0)
		{
			return {};
		}

		std::vector<VkQueueFamilyProperties> properties(property_count);
		vkGetPhysicalDeviceQueueFamilyProperties(
			physical_device, &property_count, properties.data());

		Device::QueueFamilies queue_families = {};

		uint32_t i = 0;
		for (const VkQueueFamilyProperties &queue_family : properties)
		{
			if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queue_families.graphics_family = i;
			}

			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(
				physical_device, i, m_surface, &present_support);
			if (present_support)
			{
				queue_families.present_family = i;
			}

			if (queue_families.graphics_family.has_value())
			{
				break;
			}

			++i;
		}

		return queue_families;
	}

	Expected<Device *> Device::create(VkInstance instance, VkSurfaceKHR surface)
	{
		assert(instance != nullptr);
		assert(surface != nullptr);

		Error error = Error::success();
		Device *device = new Device(instance, surface, error);
		if (error.is_error())
		{
			delete device;
			return error;
		}

		return device;
	}
} // namespace HyperEngine::Rendering
