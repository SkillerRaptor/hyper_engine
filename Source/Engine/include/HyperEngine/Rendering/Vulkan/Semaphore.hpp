/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

namespace HyperEngine::Vulkan
{
	class CSemaphore
	{
	public:
		struct SDescription
		{
			VkDevice device{ VK_NULL_HANDLE };
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

		[[nodiscard]] auto semaphore() const noexcept -> const VkSemaphore&;

	private:
		VkDevice m_device{ VK_NULL_HANDLE };

		VkSemaphore m_semaphore{ VK_NULL_HANDLE };
	};
} // namespace HyperEngine::Vulkan
