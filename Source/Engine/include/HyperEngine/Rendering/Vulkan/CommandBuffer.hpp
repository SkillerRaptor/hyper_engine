/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Rendering/Vulkan/Device.hpp"

#include <volk.h>

#include <optional>

namespace HyperEngine::Vulkan
{
	class CCommandBuffer
	{
	public:
		struct SDescription
		{
			VkDevice device{ VK_NULL_HANDLE };

			VkCommandPoolCreateFlags pool_usage{ VK_COMMAND_POOL_CREATE_FLAG_BITS_MAX_ENUM };
			VkCommandBufferLevel buffer_level{ VK_COMMAND_BUFFER_LEVEL_MAX_ENUM };

			std::optional<uint32_t> queue_family_index{ std::nullopt };
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

		auto begin(const VkCommandBufferUsageFlags buffer_usage) const -> bool;
		auto end() const -> bool;

		auto reset() const -> bool;

		[[nodiscard]] auto command_pool() const noexcept -> const VkCommandPool&;
		[[nodiscard]] auto command_buffer() const noexcept -> const VkCommandBuffer&;

	private:
		auto create_command_pool(
			const VkCommandPoolCreateFlags command_pool_create_flags,
			const uint32_t queue_family_index) -> bool;
		auto create_command_buffer(const VkCommandBufferLevel buffer_level) -> bool;

	private:
		VkDevice m_device{ VK_NULL_HANDLE };

		VkCommandPool m_command_pool{ VK_NULL_HANDLE };
		VkCommandBuffer m_command_buffer{ VK_NULL_HANDLE };
	};
} // namespace HyperEngine::Vulkan
