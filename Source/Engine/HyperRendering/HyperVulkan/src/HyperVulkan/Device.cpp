/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/Device.hpp"

#include "HyperVulkan/Context.hpp"
#include "HyperVulkan/Instance.hpp"
#include "HyperVulkan/Surface.hpp"
#include "HyperVulkan/Swapchain.hpp"

#include <set>

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
		vkEnumeratePhysicalDevices(m_context.instance()->instance(), &available_physical_device_count, nullptr);

		if (available_physical_device_count == 0)
		{
			HyperCore::Logger::fatal("Failed to find a physical device with Vulkan support");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

		std::vector<VkPhysicalDevice> available_physical_devices(available_physical_device_count);
		vkEnumeratePhysicalDevices(m_context.instance()->instance(), &available_physical_device_count, available_physical_devices.data());

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

		std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos;
		std::set<uint32_t> unique_queue_families = {
			queue_family_indices.graphics_family.value(),
			queue_family_indices.presentation_family.value()
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

#if HYPERENGINE_DEBUG
		device_create_info.ppEnabledLayerNames = Instance::s_validation_layers.data();
		device_create_info.enabledLayerCount = static_cast<uint32_t>(Instance::s_validation_layers.size());
#endif

		if (vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("Failed to create Vulkan device");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

		vkGetDeviceQueue(m_device, queue_family_indices.graphics_family.value(), 0, &m_graphics_queue);
		vkGetDeviceQueue(m_device, queue_family_indices.presentation_family.value(), 0, &m_presentation_queue);

		volkLoadDevice(m_device);

		HyperCore::Logger::debug("Vulkan device was created");

		return {};
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
		auto are_extensions_supported = check_physical_device_extension_support(physical_device);

		bool is_swapchain_supported = false;
		if (are_extensions_supported)
		{
			auto swapchain_support_details = m_context.swap_chain()->query_swap_chain_support(physical_device);
			is_swapchain_supported = !swapchain_support_details.surface_formats.empty() && !swapchain_support_details.surface_presentation_modes.empty();
		}

		return queue_family_indices.is_complete() && are_extensions_supported && is_swapchain_supported;
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

			VkBool32 presentation_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, m_context.surface()->surface(), &presentation_support);

			if (presentation_support)
			{
				queue_family_indices.presentation_family = i;
			}

			if (queue_family_indices.is_complete())
			{
				break;
			}

			++i;
		}

		return queue_family_indices;
	}

	auto Device::physical_device() const -> VkPhysicalDevice
	{
		return m_physical_device;
	}

	auto Device::device() const -> VkDevice
	{
		return m_device;
	}
	
	auto Device::graphics_queue() const -> VkQueue
	{
		return m_graphics_queue;
	}
	
	auto Device::presentation_queue() const -> VkQueue
	{
		return m_presentation_queue;
	}

	auto Device::QueueFamilyIndices::is_complete() const -> bool
	{
		return graphics_family.has_value() && presentation_family.has_value();
	}
} // namespace HyperRendering::Vulkan
