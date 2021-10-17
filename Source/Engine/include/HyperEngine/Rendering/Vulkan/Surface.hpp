/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

struct GLFWwindow;

namespace HyperEngine::Vulkan
{
	class CSurface
	{
	public:
		struct SDescription
		{
			GLFWwindow* window{ nullptr };

			VkInstance instance{ VK_NULL_HANDLE };
		};

	public:
		auto create(const SDescription& description) -> bool;
		auto destroy() -> void;

		[[nodiscard]] auto surface() const noexcept -> const VkSurfaceKHR&;

	private:
		VkInstance m_instance{ VK_NULL_HANDLE };

		VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
	};
} // namespace HyperEngine::Vulkan
