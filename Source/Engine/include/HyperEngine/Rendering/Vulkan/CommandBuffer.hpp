/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Rendering/Vulkan/Device.hpp"

#include <volk.h>

namespace HyperEngine::Vulkan
{
	class CCommandBuffer
	{
	public:
		struct SDescription
		{
			VkDevice device{ VK_NULL_HANDLE };

			CDevice::SQueueFamilies queue_families{};
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

	private:
		auto create_command_pool() -> bool;
		auto create_command_buffer() -> bool;

	private:
		VkDevice m_device{ VK_NULL_HANDLE };
		CDevice::SQueueFamilies m_queue_families{};

		VkCommandPool m_command_pool{ VK_NULL_HANDLE };
		VkCommandBuffer m_command_buffer{ VK_NULL_HANDLE };
	};
} // namespace HyperEngine::Vulkan
