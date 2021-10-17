/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/CommandBuffer.hpp"

#include "HyperEngine/Core/Assertion.hpp"

namespace HyperEngine::Vulkan
{
	auto CCommandBuffer::create(const CCommandBuffer::SDescription& description) -> bool
	{
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.device, VK_NULL_HANDLE);
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.pool_usage, VK_COMMAND_POOL_CREATE_FLAG_BITS_MAX_ENUM);
		HYPERENGINE_ASSERT_IS_NOT_EQUAL(description.buffer_level, VK_COMMAND_BUFFER_LEVEL_MAX_ENUM);
		HYPERENGINE_ASSERT_IS_TRUE(description.queue_family_index.has_value());

		m_device = description.device;

		if (!create_command_pool(description.pool_usage, description.queue_family_index.value()))
		{
			CLogger::fatal("CCommandBuffer::create(): Failed to create command pool");
			return false;
		}

		if (!create_command_buffer(description.buffer_level))
		{
			CLogger::fatal("CCommandBuffer::create(): Failed to create command buffer");
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

	auto CCommandBuffer::create_command_pool(
		const VkCommandPoolCreateFlags command_pool_create_flags,
		const uint32_t queue_family_index) -> bool
	{
		VkCommandPoolCreateInfo command_pool_create_info{};
		command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_create_info.pNext = nullptr;
		command_pool_create_info.flags = command_pool_create_flags;
		command_pool_create_info.queueFamilyIndex = queue_family_index;

		if (vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_command_pool) != VK_SUCCESS)
		{
			CLogger::fatal("CCommandBuffer::create_command_pool(): Failed to create command pool");
			return false;
		}

		return true;
	}

	auto CCommandBuffer::create_command_buffer(const VkCommandBufferLevel buffer_level) -> bool
	{
		VkCommandBufferAllocateInfo command_buffer_allocate_info{};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.pNext = nullptr;
		command_buffer_allocate_info.commandPool = m_command_pool;
		command_buffer_allocate_info.level = buffer_level;
		command_buffer_allocate_info.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &m_command_buffer) != VK_SUCCESS)
		{
			CLogger::fatal("CCommandBuffer::create_command_buffer(): Failed to create command buffer");
			return false;
		}

		return true;
	}

	auto CCommandBuffer::begin(const VkCommandBufferUsageFlags buffer_usage) const -> bool
	{
		VkCommandBufferBeginInfo command_buffer_begin_info{};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_begin_info.pNext = nullptr;
		command_buffer_begin_info.flags = buffer_usage;
		command_buffer_begin_info.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(m_command_buffer, &command_buffer_begin_info) != VK_SUCCESS)
		{
			CLogger::fatal("CCommandBuffer::begin(): Failed to begin command buffer");
			return false;
		}

		return true;
	}

	auto CCommandBuffer::end() const -> bool
	{
		if (vkEndCommandBuffer(m_command_buffer) != VK_SUCCESS)
		{
			CLogger::fatal("CCommandBuffer::end(): Failed to end command buffer");
			return false;
		}

		return true;
	}

	auto CCommandBuffer::reset() const -> bool
	{
		if (vkResetCommandBuffer(m_command_buffer, 0) != VK_SUCCESS)
		{
			CLogger::fatal("CCommandBuffer::reset(): Failed to reset command buffer");
			return false;
		}

		return true;
	}

	auto CCommandBuffer::command_pool() const noexcept -> const VkCommandPool&
	{
		return m_command_pool;
	}

	auto CCommandBuffer::command_buffer() const noexcept -> const VkCommandBuffer&
	{
		return m_command_buffer;
	}
} // namespace HyperEngine::Vulkan
