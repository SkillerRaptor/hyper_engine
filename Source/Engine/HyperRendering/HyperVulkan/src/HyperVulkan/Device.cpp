/*
 * Copyright (c) 2020-$YEAR, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/Device.hpp"

#include <HyperCore/Logger.hpp>

#include <set>
#include <vector>

namespace HyperRendering::HyperVulkan
{
	Device::Device(VkInstance& t_instance, VkSurfaceKHR& t_surface)
		: m_instance(t_instance)
		, m_surface(t_surface)
	{
	}

	auto Device::initialize() -> bool
	{
		if (!pick_physical_device())
		{
			HyperCore::Logger::fatal("Device::initialize(): Failed to pick physical Vulkan device");
			return false;
		}

		if (!create_device())
		{
			HyperCore::Logger::fatal("Device::initialize(): Failed to create Vulkan device");
			return false;
		}

		return true;
	}

	auto Device::destroy() -> bool
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
			HyperCore::Logger::fatal("Device::pick_physical_device(): Failed to find a suitable physical Vulkan device");
			return false;
		}

		HyperCore::Logger::debug("Physical Vulkan device was found");

		return true;
	}

	auto Device::create_device() -> bool
	{
		auto queue_family_indices = find_queue_families(m_physical_device);

		std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos;
		std::set<uint32_t> unique_queue_families = {
			queue_family_indices.graphics_family.value(),
			queue_family_indices.present_family.value()
		};

		auto queue_priority = 1.0F;
		for (auto queue_family : unique_queue_families)
		{
			VkDeviceQueueCreateInfo device_queue_create_info{};
			device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			device_queue_create_info.queueFamilyIndex = queue_family;
			device_queue_create_info.pQueuePriorities = &queue_priority;
			device_queue_create_info.queueCount = 1;

			device_queue_create_infos.push_back(device_queue_create_info);
		}

		VkPhysicalDeviceFeatures physical_device_features{};

		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.pEnabledFeatures = &physical_device_features;
		device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(device_queue_create_infos.size());
		device_create_info.ppEnabledExtensionNames = s_device_extensions.data();
		device_create_info.enabledExtensionCount = static_cast<uint32_t>(s_device_extensions.size());

		if (vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("Device::create_device(): Failed to create Vulkan device");
			return false;
		}

		vkGetDeviceQueue(m_device, queue_family_indices.graphics_family.value(), 0, &m_graphics_queue);
		vkGetDeviceQueue(m_device, queue_family_indices.present_family.value(), 0, &m_present_queue);

		volkLoadDevice(m_device);

		HyperCore::Logger::debug("Vulkan device was created");

		return true;
	}
	
	auto Device::check_physical_device_extension_support(VkPhysicalDevice physical_device) const -> bool
	{
		uint32_t available_device_extension_count = 0;
		vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &available_device_extension_count, nullptr);

		std::vector<VkExtensionProperties> available_device_extensions(available_device_extension_count);
		vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &available_device_extension_count, available_device_extensions.data());

		std::set<std::string> required_extensions(s_device_extensions.begin(), s_device_extensions.end());

		for (const auto& device_extension : available_device_extensions)
		{
			required_extensions.erase(device_extension.extensionName);
		}

		return required_extensions.empty();
	}

	auto Device::is_physical_device_suitable(VkPhysicalDevice physical_device) const -> bool
	{
		auto queue_family_indices = find_queue_families(physical_device);
		auto extensions_supported = check_physical_device_extension_support(physical_device);

		auto swapchain_supported = false;
		if (extensions_supported)
		{
			auto surface_formats = get_surface_formats(physical_device);
			auto surface_present_modes = get_surface_present_modes(physical_device);
			swapchain_supported = !surface_formats.empty() && !surface_present_modes.empty();
		}
		
		return queue_family_indices.is_complete() && extensions_supported && swapchain_supported;
	}
	
	auto Device::physical_device() const -> const VkPhysicalDevice&
	{
		return m_physical_device;
	}
	
	auto Device::device() const -> const VkDevice&
	{
		return m_device;
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

			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, m_surface, &present_support);

			if (present_support)
			{
				queue_family_indices.present_family = i;
			}

			if (queue_family_indices.is_complete())
			{
				break;
			}

			++i;
		}

		return queue_family_indices;
	}

	auto Device::get_surface_capabilities(VkPhysicalDevice physical_device) const -> VkSurfaceCapabilitiesKHR
	{
		VkSurfaceCapabilitiesKHR surface_capabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			physical_device,
			m_surface,
			&surface_capabilities);

		return surface_capabilities;
	}

	auto Device::get_surface_formats(VkPhysicalDevice physical_device) const -> std::vector<VkSurfaceFormatKHR>
	{
		std::vector<VkSurfaceFormatKHR> surface_formats;

		uint32_t surface_format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			physical_device,
			m_surface,
			&surface_format_count,
			nullptr);

		if (surface_format_count != 0)
		{
			surface_formats.resize(surface_format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(
				physical_device,
				m_surface,
				&surface_format_count,
				surface_formats.data());
		}

		return surface_formats;
	}

	auto Device::get_surface_present_modes(VkPhysicalDevice physical_device) const -> std::vector<VkPresentModeKHR>
	{
		std::vector<VkPresentModeKHR> surface_present_modes;

		uint32_t surface_present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			physical_device,
			m_surface,
			&surface_present_mode_count,
			nullptr);

		if (surface_present_mode_count != 0)
		{
			surface_present_modes.resize(surface_present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				physical_device,
				m_surface,
				&surface_present_mode_count,
				surface_present_modes.data());
		}

		return surface_present_modes;
	}

	auto Device::QueueFamilyIndices::is_complete() const -> bool
	{
		return graphics_family.has_value() && present_family.has_value();
	}
} // namespace HyperRendering::HyperVulkan
