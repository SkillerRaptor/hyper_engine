/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

namespace HyperEngine::Vulkan
{
	class CFence
	{
	public:
		struct SDescription
		{
			VkDevice device{ VK_NULL_HANDLE };
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

		auto wait() const -> bool;
		auto reset() const -> bool;

		[[nodiscard]] auto fence() const noexcept -> const VkFence&;

	private:
		VkDevice m_device{ VK_NULL_HANDLE };

		VkFence m_fence{ VK_NULL_HANDLE };
	};
} // namespace HyperEngine::Vulkan
