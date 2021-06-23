/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

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
		struct SQueueFamilies
		{
			std::optional<uint32_t> graphics_family{ std::nullopt };
			
			bool complete() const;
		};

	public:
		bool select_physical_device(const CContext& context);

		SQueueFamilies get_queue_families() const;
		
		const VkPhysicalDevice& physical_device() const;

	private:
		static bool is_physical_device_suitable(
			const VkPhysicalDevice& physical_device);

		static SQueueFamilies
			find_queue_families(const VkPhysicalDevice& physical_device);

	private:
		VkPhysicalDevice m_physical_device{ nullptr };
	};

	class CDevice
	{
	public:
		bool initialize(const CContext& context);
		void shutdown();
		
	private:
		bool create_logical_device(const CContext& context);

	private:
		VkDevice m_logical_device{ nullptr };
		
		VkQueue m_graphics_queue{ nullptr };
		
		CGpu m_gpu{};
	};
} // namespace HyperRendering::Vulkan
