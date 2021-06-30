/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Logger.hpp>
#include <HyperRendering/Vulkan/Context.hpp>
#include <HyperRendering/Vulkan/IPlatformContext.hpp>
#include <HyperRendering/Vulkan/SwapChain.hpp>
#include <vulkan/vulkan.h>

namespace HyperRendering::Vulkan
{
	bool CSwapChain::initialize(const CContext* context)
	{
		m_context = context;

		if (!context->platform_context()->create_window_surface(
				context->instance(), m_surface))
		{
			HyperCore::CLogger::error("Vulkan: failed to create window surface!");
			return false;
		}

		return true;
	}

	void CSwapChain::shutdown()
	{
		//vkDestroySwapchainKHR(
		//	m_context->device().logical_device(), m_swap_chain, nullptr);
		vkDestroySurfaceKHR(m_context->instance(), m_surface, nullptr);
	}

	const VkSurfaceKHR& CSwapChain::surface() const
	{
		return m_surface;
	}
} // namespace HyperRendering::Vulkan
