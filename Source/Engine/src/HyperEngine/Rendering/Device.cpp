/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Device.hpp"

#include <set>
#include <vector>
#include <volk.h>

namespace HyperEngine
{
	Device::Device(VkInstance instance, VkSurfaceKHR surface, Error &error)
		: m_instance(instance)
		, m_surface(surface)
	{
		const auto physical_device = find_physical_device();
		if (physical_device.is_error())
		{
			error = physical_device.error();
			return;
		}

		const auto device = create_device();
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
		m_physical_device = std::exchange(other.m_physical_device, nullptr);
		m_device = std::exchange(other.m_device, nullptr);
	}

	Device &Device::operator=(Device &&other) noexcept
	{
		m_physical_device = std::exchange(other.m_physical_device, nullptr);
		m_device = std::exchange(other.m_device, nullptr);

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
			.enabledExtensionCount =
				static_cast<uint32_t>(s_device_extensions.size()),
			.ppEnabledExtensionNames = s_device_extensions.data(),
			.pEnabledFeatures = &physical_device_features,
		};

		const auto device_result = vkCreateDevice(
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
		const bool extensions_supported =
			check_physical_device_extensions_support(physical_device);
		const bool swap_chain_adequate =
			[this, physical_device, extensions_supported]()
		{
			if (!extensions_supported)
			{
				return false;
			}

			const SwapChainSupportDetails swap_chain_support_details =
				query_swap_chain_support(physical_device);
			return !swap_chain_support_details.formats.empty() &&
						 !swap_chain_support_details.present_modes.empty();
		}();

		return queue_families.graphics_family.has_value() &&
					 queue_families.present_family.has_value() && extensions_supported &&
					 swap_chain_adequate;
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

	Device::SwapChainSupportDetails Device::query_swap_chain_support(
		VkPhysicalDevice physical_device) const
	{
		SwapChainSupportDetails swap_chain_support_details = {};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			physical_device, m_surface, &swap_chain_support_details.capabilities);

		uint32_t format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			physical_device, m_surface, &format_count, nullptr);
		if (format_count != 0)
		{
			swap_chain_support_details.formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(
				physical_device,
				m_surface,
				&format_count,
				swap_chain_support_details.formats.data());
		}

		uint32_t present_mode_count = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			physical_device, m_surface, &present_mode_count, nullptr);
		if (present_mode_count != 0)
		{
			swap_chain_support_details.present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				physical_device,
				m_surface,
				&present_mode_count,
				swap_chain_support_details.present_modes.data());
		}

		return swap_chain_support_details;
	}

	bool Device::check_physical_device_extensions_support(
		VkPhysicalDevice physical_device) const
	{
		uint32_t extension_count = 0;
		vkEnumerateDeviceExtensionProperties(
			physical_device, nullptr, &extension_count, nullptr);
		if (extension_count == 0)
		{
			return false;
		}

		std::vector<VkExtensionProperties> extensions(extension_count);
		vkEnumerateDeviceExtensionProperties(
			physical_device, nullptr, &extension_count, extensions.data());

		std::set<std::string> required_extensions(
			s_device_extensions.begin(), s_device_extensions.end());

		for (const VkExtensionProperties &extension : extensions)
		{
			required_extensions.erase(extension.extensionName);
		}

		return required_extensions.empty();
	}

	VkPhysicalDevice Device::physical_device() const
	{
		return m_physical_device;
	}

	VkDevice Device::device() const
	{
		return m_device;
	}

	Expected<Device *> Device::create(VkInstance instance, VkSurfaceKHR surface)
	{
		assert(instance != nullptr);
		assert(surface != nullptr);

		Error error = Error::success();
		auto *device = new Device(instance, surface, error);
		if (error.is_error())
		{
			delete device;
			return error;
		}

		return device;
	}
} // namespace HyperEngine
