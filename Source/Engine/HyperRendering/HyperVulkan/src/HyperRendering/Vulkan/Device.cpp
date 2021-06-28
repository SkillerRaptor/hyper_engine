/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Logger.hpp>
#include <HyperRendering/Vulkan/Context.hpp>
#include <HyperRendering/Vulkan/Device.hpp>
#include <set>
#include <vulkan/vulkan.h>

namespace HyperRendering::Vulkan
{
	bool CGpu::SQueueFamilies::complete() const
	{
		return graphics_family.has_value() && presentation_family.has_value();
	}

	bool CGpu::initialize(const CContext* context)
	{
		m_context = context;

		if (!select_physical_device())
		{
			return false;
		}

		return true;
	}

	bool CGpu::select_physical_device()
	{
		uint32_t physical_device_count = 0;
		vkEnumeratePhysicalDevices(
			m_context->instance(), &physical_device_count, nullptr);

		if (physical_device_count == 0)
		{
			HyperCore::CLogger::fatal(
				"Vulkan: Failed to find GPUs with vulkan support!");
			return false;
		}

		std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
		vkEnumeratePhysicalDevices(
			m_context->instance(),
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
			HyperCore::CLogger::fatal("Vulkan: failed to find suitable GPU!");
			return false;
		}

		return true;
	}

	bool CGpu::is_physical_device_suitable(
		const VkPhysicalDevice& physical_device) const
	{
		CGpu::SQueueFamilies queue_families =
			find_queue_families(physical_device);

		bool device_extensions_supported =
			check_device_extension_support(physical_device);

		return queue_families.complete();
	}

	CGpu::SQueueFamilies
		CGpu::find_queue_families(const VkPhysicalDevice& physical_device) const
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

			VkBool32 presentation_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(
				physical_device,
				index,
				m_context->swap_chain().surface(),
				&presentation_support);

			if (presentation_support)
			{
				queue_families.presentation_family = index;
			}

			if (queue_families.complete())
			{
				break;
			}

			++index;
		}

		return queue_families;
	}

	bool CGpu::check_device_extension_support(
		const VkPhysicalDevice& physical_device)
	{
		uint32_t available_extension_properties_count = 0;
		vkEnumerateDeviceExtensionProperties(
			physical_device,
			nullptr,
			&available_extension_properties_count,
			nullptr);

		std::vector<VkExtensionProperties> available_extensions_properties(
			available_extension_properties_count);
		vkEnumerateDeviceExtensionProperties(
			physical_device,
			nullptr,
			&available_extension_properties_count,
			available_extensions_properties.data());

		std::set<std::string> required_extensions(
			s_device_extensions.begin(), s_device_extensions.end());

		for (const VkExtensionProperties& extension_properties :
			 available_extensions_properties)
		{
			required_extensions.erase(extension_properties.extensionName);
		}

		return required_extensions.empty();
	}

	CGpu::SQueueFamilies CGpu::get_queue_families() const
	{
		return find_queue_families(m_physical_device);
	}

	const VkPhysicalDevice& CGpu::physical_device() const
	{
		return m_physical_device;
	}

	bool CDevice::initialize(const CContext* context)
	{
		m_context = context;

		if (!m_gpu.initialize(m_context))
		{
			return false;
		}

		if (!create_logical_device())
		{
			return false;
		}

		return true;
	}

	void CDevice::shutdown()
	{
		vkDestroyDevice(m_logical_device, nullptr);
	}

	bool CDevice::create_logical_device()
	{
		CGpu::SQueueFamilies queue_families = m_gpu.get_queue_families();

		std::set<uint32_t> unique_queue_families = {
			queue_families.graphics_family.value(),
			queue_families.presentation_family.value()
		};

		float queue_priority = 1.0f;

		std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos;
		for (uint32_t queue_family : unique_queue_families)
		{
			VkDeviceQueueCreateInfo device_queue_create_info{};
			device_queue_create_info.sType =
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			device_queue_create_info.queueFamilyIndex = queue_family;
			device_queue_create_info.queueCount = 1;
			device_queue_create_info.pQueuePriorities = &queue_priority;

			device_queue_create_infos.push_back(device_queue_create_info);
		}

		VkPhysicalDeviceFeatures physical_device_features{};

		uint32_t layer_count = 0;
		const char* const* layers = nullptr;

		if (m_context->is_validation_layer_enabled())
		{
			layer_count =
				static_cast<uint32_t>(CContext::s_validation_layers.size());
			layers = CContext::s_validation_layers.data();
		}

		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.queueCreateInfoCount =
			static_cast<uint32_t>(device_queue_create_infos.size());
		device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
		device_create_info.pEnabledFeatures = &physical_device_features;
		device_create_info.enabledExtensionCount =
			static_cast<uint32_t>(CGpu::s_device_extensions.size());
		device_create_info.ppEnabledExtensionNames =
			CGpu::s_device_extensions.data();
		device_create_info.enabledLayerCount = layer_count;
		device_create_info.ppEnabledLayerNames = layers;

		if (vkCreateDevice(
				m_gpu.physical_device(),
				&device_create_info,
				nullptr,
				&m_logical_device) != VK_SUCCESS)
		{
			HyperCore::CLogger::fatal(
				"Vulkan: failed to create logical device!");
			return false;
		}

		vkGetDeviceQueue(
			m_logical_device,
			queue_families.graphics_family.value(),
			0,
			&m_graphics_queue);

		vkGetDeviceQueue(
			m_logical_device,
			queue_families.presentation_family.value(),
			0,
			&m_presentation_queue);

		return true;
	}

	const VkDevice& CDevice::logical_device() const
	{
		return m_logical_device;
	}
} // namespace HyperRendering::Vulkan
