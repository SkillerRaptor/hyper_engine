/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <array>
#include <optional>
#include <vector>

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
			std::optional<uint32_t> present_family;

			auto is_complete() const -> bool;
		};

	public:
		Device(VkInstance& t_instance, VkSurfaceKHR& t_surface);

		auto initialize() -> bool;
		auto destroy() -> bool;

		auto physical_device() -> VkPhysicalDevice&;
		auto device() -> VkDevice&;
		
		auto graphics_queue() -> VkQueue&;
		auto present_queue() -> VkQueue&;

		auto find_queue_families(VkPhysicalDevice physical_device) const -> QueueFamilyIndices;

		auto get_surface_capabilities(VkPhysicalDevice physical_device) const -> VkSurfaceCapabilitiesKHR;
		auto get_surface_formats(VkPhysicalDevice physical_device) const -> std::vector<VkSurfaceFormatKHR>;
		auto get_surface_present_modes(VkPhysicalDevice physical_device) const -> std::vector<VkPresentModeKHR>;

	private:
		auto pick_physical_device() -> bool;
		auto create_device() -> bool;

		auto check_physical_device_extension_support(VkPhysicalDevice physical_device) const -> bool;
		auto is_physical_device_suitable(VkPhysicalDevice physical_device) const -> bool;

	private:
		VkInstance& m_instance;
		VkSurfaceKHR& m_surface;

		VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
		VkDevice m_device{ VK_NULL_HANDLE };

		VkQueue m_graphics_queue{ VK_NULL_HANDLE };
		VkQueue m_present_queue{ VK_NULL_HANDLE };
	};
} // namespace HyperRendering::HyperVulkan
