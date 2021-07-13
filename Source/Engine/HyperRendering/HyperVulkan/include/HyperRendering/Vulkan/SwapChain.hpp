/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

using VkSurfaceKHR = struct VkSurfaceKHR_T*;
using VkSwapchainKHR = struct VkSwapchainKHR_T*;

namespace HyperRendering::Vulkan
{
	class CContext;

	class CSwapChain
	{
	public:
		bool initialize(const CContext* context);
		void shutdown();
		
		const VkSurfaceKHR& surface() const;

	private:
		const CContext* m_context{ nullptr };
		
		VkSurfaceKHR m_surface{ nullptr };
		VkSwapchainKHR m_swap_chain{ nullptr };
	};
} // namespace HyperRendering::Vulkan
