/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Logger.hpp>
#include <HyperRendering/Vulkan/Context.hpp>
#include <HyperRendering/Vulkan/Device.hpp>
#include <vulkan/vulkan.h>

namespace HyperRendering::Vulkan
{
	bool CGpu::SQueueFamilies::complete() const
	{
		return graphics_family.has_value();
	}

	bool CGpu::select_physical_device(const CContext& context)
	{
		uint32_t physical_device_count = 0;
		vkEnumeratePhysicalDevices(
			context.instance(), &physical_device_count, nullptr);

		if (physical_device_count == 0)
		{
			HyperCore::CLogger::fatal(
				"Failed to find GPUs with vulkan support!");
			return false;
		}

		std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
		vkEnumeratePhysicalDevices(
			context.instance(),
			&physical_device_count,
			physical_devices.data());

		for (const VkPhysicalDevice& physical_device : physical_devices)
		{
			if (is_physical_device_suitable(physical_device))
			{
				m_physical_device = physical_device;
				break;
			}
		}

		if (m_physical_device == VK_NULL_HANDLE)
		{
			HyperCore::CLogger::fatal("Failed to find suitable vulkan GPU!");
			return false;
		}

		return true;
	}

	bool CGpu::is_physical_device_suitable(
		const VkPhysicalDevice& physical_device)
	{
		CGpu::SQueueFamilies queue_families =
			find_queue_families(physical_device);
		return queue_families.complete();
	}

	CGpu::SQueueFamilies
		CGpu::find_queue_families(const VkPhysicalDevice& physical_device)
	{
		CGpu::SQueueFamilies queue_families{};

		uint32_t available_queue_family_properties_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(
			physical_device, &available_queue_family_properties_count, nullptr);

		std::vector<VkQueueFamilyProperties> available_queue_family_properties(
			available_queue_family_properties_count);
		vkGetPhysicalDeviceQueueFamilyProperties(
			physical_device,
			&available_queue_family_properties_count,
			available_queue_family_properties.data());

		uint32_t index = 0;
		for (const VkQueueFamilyProperties& queue_family_properties :
			 available_queue_family_properties)
		{
			if (queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queue_families.graphics_family = index;
			}

			if (queue_families.complete())
			{
				break;
			}

			++index;
		}

		return queue_families;
	}

	const VkPhysicalDevice& CGpu::physical_device() const
	{
		return m_physical_device;
	}

	bool CDevice::initialize(const CContext& context)
	{
		if (!m_gpu.select_physical_device(context))
		{
			return false;
		}

		return true;
	}

	void CDevice::shutdown()
	{
	}
} // namespace HyperRendering::Vulkan
