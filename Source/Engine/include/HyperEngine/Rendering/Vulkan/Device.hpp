/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <optional>

namespace HyperEngine::Vulkan
{
	class CDevice
	{
	public:
		struct SDescription
		{
			VkInstance instance{ VK_NULL_HANDLE };
			VkSurfaceKHR surface{ VK_NULL_HANDLE };
		};

		struct SQueueFamilies
		{
			std::optional<uint32_t> graphics_family{ std::nullopt };
			std::optional<uint32_t> present_family{ std::nullopt };
		};

		struct SQueues
		{
			VkQueue graphics_queue{ VK_NULL_HANDLE };
			VkQueue present_queue{ VK_NULL_HANDLE };
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

	private:
		auto select_physical_device() -> bool;
		auto create_logical_device() -> bool;

		[[nodiscard]] auto is_physical_device_suitable(VkPhysicalDevice physical_device) const -> bool;
		[[nodiscard]] auto find_queue_families(VkPhysicalDevice physical_device) const -> SQueueFamilies;

	private:
		VkInstance m_instance{ VK_NULL_HANDLE };
		VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
		
		VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };
		VkDevice m_device{ VK_NULL_HANDLE };

		SQueues m_queues{};
	};
} // namespace HyperEngine::Vulkan
