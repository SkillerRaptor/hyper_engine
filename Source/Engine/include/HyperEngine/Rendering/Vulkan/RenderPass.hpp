/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <vector>

namespace HyperEngine::Vulkan
{
	class CCommandBuffer;
	class CFrameBuffer;
	class CSwapchain;
	
	class CRenderPass
	{
	public:
		struct SDescription
		{
			VkDevice device{ VK_NULL_HANDLE };

			VkFormat image_format{ VK_FORMAT_MAX_ENUM };
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

		auto begin(
			const CCommandBuffer& command_buffer,
			const CFrameBuffer& frame_buffer,
			const CSwapchain& swapchain,
			const VkClearValue clear_value) const -> void;
		auto end(const CCommandBuffer& command_buffer) const -> void;

		[[nodiscard]] auto render_pass() const noexcept -> const VkRenderPass&;

	private:
		VkDevice m_device{ VK_NULL_HANDLE };

		VkRenderPass m_render_pass{ VK_NULL_HANDLE };
	};
} // namespace HyperEngine::Vulkan
