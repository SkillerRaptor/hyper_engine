/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/CommandBuffer.hpp"

#include "HyperEngine/Core/Logger.hpp"

namespace HyperEngine::Vulkan
{
	auto CCommandBuffer::create(const CCommandBuffer::SDescription& description) -> bool
	{
		if (description.device == VK_NULL_HANDLE)
		{
			CLogger::fatal("CCommandBuffer::create(): The description vulkan device is null");
			return false;
		}

		m_device = description.device;
		m_queue_families = description.queue_families;

		if (!create_command_pool())
		{
			CLogger::fatal("CCommandBuffer::create(): Failed to create vulkan command pool");
			return false;
		}

		if (!create_command_buffer())
		{
			CLogger::fatal("CCommandBuffer::create(): Failed to create vulkan command buffer");
			return false;
		}

		return true;
	}

	auto CCommandBuffer::destroy() -> void
	{
		if (m_command_pool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(m_device, m_command_pool, nullptr);
		}
	}

	auto CCommandBuffer::create_command_pool() -> bool
	{
		VkCommandPoolCreateInfo command_pool_create_info{};
		command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_create_info.pNext = nullptr;
		command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		command_pool_create_info.queueFamilyIndex = m_queue_families.graphics_family.value();

		if (vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_command_pool) != VK_SUCCESS)
		{
			CLogger::fatal("CCommandBuffer::create_command_pool(): Failed to create vulkan command pool");
			return false;
		}

		return true;
	}

	auto CCommandBuffer::create_command_buffer() -> bool
	{
		VkCommandBufferAllocateInfo command_buffer_allocate_info{};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.pNext = nullptr;
		command_buffer_allocate_info.commandPool = m_command_pool;
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &m_command_buffer) != VK_SUCCESS)
		{
			CLogger::fatal("CCommandBuffer::create_command_buffer(): Failed to create vulkan command buffer");
			return false;
		}

		return true;
	}
} // namespace HyperEngine::Vulkan
