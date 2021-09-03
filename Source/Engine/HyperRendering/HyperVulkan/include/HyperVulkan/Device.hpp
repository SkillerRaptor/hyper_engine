/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <array>
#include <optional>

namespace HyperRendering::HyperVulkan
{
	class GraphicsContext;
	
	class Device
	{
	public:
		static constexpr std::array<const char*, 1> s_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		
	private:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics_family;
			
			auto is_complete() const -> bool;
		};
		
	public:
		explicit Device(VkInstance& t_instance);
		
		auto initialize() -> bool;
		auto terminate() -> bool;
		
	private:
		auto pick_physical_device() -> bool;
		auto is_physical_device_suitable(VkPhysicalDevice physical_device) const -> bool;
		
		auto create_device() -> bool;
		
		auto find_queue_families(VkPhysicalDevice physical_device) const -> QueueFamilyIndices;
		
	private:
		VkInstance& m_instance;
		
		VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
		VkDevice m_device{ VK_NULL_HANDLE };
		
		VkQueue m_graphics_queue{ VK_NULL_HANDLE };
	};
} // namespace HyperRendering::HyperVulkan
