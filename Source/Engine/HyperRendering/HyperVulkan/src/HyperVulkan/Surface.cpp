/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/Surface.hpp"

#include "HyperVulkan/Context.hpp"

#include <volk.h>
#include <GLFW/glfw3.h>

namespace HyperRendering::Vulkan
{
	Surface::Surface(Context& context)
		: m_context(context)
	{
	}

	auto Surface::initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
	{
		if (glfwCreateWindowSurface(m_context.instance(), m_context.window().native_window(), nullptr, &m_surface) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("Failed to create vulkan window surface");
			return HyperCore::Errors::ConstructError::Incomplete;
		}
		
		HyperCore::Logger::debug("Vulkan window surface was created");
		
		return {};
	}

	auto Surface::terminate() -> void
	{
		vkDestroySurfaceKHR(m_context.instance(), m_surface, nullptr);
		HyperCore::Logger::debug("Vulkan window surface was destroyed");
	}
	
	auto Surface::surface() const -> VkSurfaceKHR
	{
		return m_surface;
	}
} // namespace HyperRendering::Vulkan
