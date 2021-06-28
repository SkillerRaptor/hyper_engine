/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <optional>

using VkDevice = struct VkDevice_T*;
using VkPhysicalDevice = struct VkPhysicalDevice_T*;
using VkQueue = struct VkQueue_T*;

namespace HyperRendering::Vulkan
{
	class CContext;

	class CGpu
	{
	public:
		static constexpr const std::array<const char*, 1>
			s_device_extensions = { "VK_KHR_swapchain" };

	public:
		struct SQueueFamilies
		{
			std::optional<uint32_t> graphics_family{ std::nullopt };
			std::optional<uint32_t> presentation_family{ std::nullopt };

			bool complete() const;
		};

	public:
		bool initialize(const CContext* context);

		SQueueFamilies get_queue_families() const;
		const VkPhysicalDevice& physical_device() const;

	private:
		bool select_physical_device();

		bool is_physical_device_suitable(
			const VkPhysicalDevice& physical_device) const;

		SQueueFamilies
			find_queue_families(const VkPhysicalDevice& physical_device) const;
		
		static bool check_device_extension_support(
			const VkPhysicalDevice& physical_device);

	private:
		const CContext* m_context{ nullptr };

		VkPhysicalDevice m_physical_device{ nullptr };
	};

	class CDevice
	{
	public:
		bool initialize(const CContext* context);
		void shutdown();

		const VkDevice& logical_device() const;

	private:
		bool create_logical_device();

	private:
		const CContext* m_context{ nullptr };

		VkDevice m_logical_device{ nullptr };

		VkQueue m_graphics_queue{ nullptr };
		VkQueue m_presentation_queue{ nullptr };

		CGpu m_gpu{};
	};
} // namespace HyperRendering::Vulkan
