/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

namespace HyperEngine::Vulkan
{
	class CFrameBuffer
	{
	public:
		struct SDescription
		{
			VkDevice device{ VK_NULL_HANDLE };
			VkRenderPass render_pass{ VK_NULL_HANDLE };
			VkImageView swapchain_image_view{ VK_NULL_HANDLE };
			VkExtent2D swapchain_extent{};
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

	private:
		VkDevice m_device{ VK_NULL_HANDLE };

		VkFramebuffer m_framebuffer{ VK_NULL_HANDLE };
	};
} // namespace HyperEngine::Vulkan
