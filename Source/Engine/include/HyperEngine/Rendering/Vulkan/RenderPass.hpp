/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

namespace HyperEngine::Vulkan
{
	class CRenderPass
	{
	public:
		struct SDescription
		{
			VkDevice device{ VK_NULL_HANDLE };

			VkFormat swapchain_image_format{};
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

	private:
		VkDevice m_device{ VK_NULL_HANDLE };

		VkRenderPass m_render_pass{ VK_NULL_HANDLE };
	};
} // namespace HyperEngine::Vulkan
