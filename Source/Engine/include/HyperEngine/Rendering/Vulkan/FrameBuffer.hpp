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
	class CFrameBuffer
	{
	public:
		struct SDescription
		{
			VkDevice device{ VK_NULL_HANDLE };
			VkImageView image_attachment{ VK_NULL_HANDLE };
			VkRenderPass render_pass{ VK_NULL_HANDLE };

			uint32_t width{ 0 };
			uint32_t height{ 0 };
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

		[[nodiscard]] auto frame_buffer() const noexcept -> const VkFramebuffer&;

	private:
		VkDevice m_device{ VK_NULL_HANDLE };

		VkFramebuffer m_frame_buffer{ VK_NULL_HANDLE };
	};
} // namespace HyperEngine::Vulkan
